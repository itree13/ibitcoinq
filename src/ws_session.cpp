#include "ws_session.h"
#include "logger.h"
#include "socks/handshake.hpp"
#include <boost/asio/strand.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <iostream>


WSSession::WSSession(net::io_context& ioc, ssl::context& ctx,
    const std::string& host, const std::string& port, const std::string& path)
    : resolver_(net::make_strand(ioc)),
    ws_(net::make_strand(ioc), ctx),
    host_(host),
    port_(port),
    path_(path) {
}

void WSSession::start() {
    if (!socks_server_.empty()) {
        if (!socks_url_.parse(socks_server_)) {
            std::cerr << "parse socks url error\n";
            return;
        }
        // socks handshake.
        socks_version_ = socks_url_.scheme() == "socks4" ? 4 : 0;
        socks_version_ = socks_url_.scheme() == "socks5" ? 5 : socks_version_;
        if (socks_version_ == 0) {
            std::cerr << "incorrect socks version\n";
            return;
        }
        // Look up the domain name
        resolver_.async_resolve(
            std::string(socks_url_.host()),
            std::string(socks_url_.port()),
            beast::bind_front_handler(
                &WSSession::on_socks_proxy_resolve,
                shared_from_this()));
        return;
    }

    // Look up the domain name
    resolver_.async_resolve(
        host_,
        port_,
        beast::bind_front_handler(
            &WSSession::on_resolve,
            shared_from_this()));
}

void WSSession::on_socks_proxy_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec)
        return on_fail(ec, "resolve");

    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
    beast::get_lowest_layer(ws_).async_connect(
        results,
        beast::bind_front_handler(
            &WSSession::on_socks_proxy_connect,
            shared_from_this()));
}

void WSSession::on_socks_proxy_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec)
        return on_fail(ec, "proxy_connect");

     if (socks_version_ == 4)
        socks::async_handshake_v4(
            beast::get_lowest_layer(ws_),
            host_,
            static_cast<unsigned short>(std::atoi(port_.c_str())),
            std::string(socks_url_.username()),
            beast::bind_front_handler(
                &WSSession::on_socks_proxy_handshake,
                shared_from_this()));
    else
        socks::async_handshake_v5(
            beast::get_lowest_layer(ws_),
            host_,
            static_cast<unsigned short>(std::atoi(port_.c_str())),
            std::string(socks_url_.username()),
            std::string(socks_url_.password()),
            true,
            beast::bind_front_handler(
                &WSSession::on_socks_proxy_handshake,
                shared_from_this()));
}

void WSSession::on_socks_proxy_handshake(beast::error_code ec) {
    if (ec)
        return on_fail(ec, "proxy_handshake");

    on_connect(ec, tcp::resolver::results_type::endpoint_type());
}

void WSSession::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec)
        return on_fail(ec, "resolve");

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ws_).async_connect(
        results,
        beast::bind_front_handler(
            &WSSession::on_connect,
            shared_from_this()));
}

void WSSession::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec)
        return on_fail(ec, "connect");

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(
        ws_.next_layer().native_handle(),
        host_.c_str())) {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()),
            net::error::get_ssl_category());
        return on_fail(ec, "connect");
    }

    // Perform the SSL handshake
    ws_.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &WSSession::on_ssl_handshake,
            shared_from_this()));
}

void WSSession::on_ssl_handshake(beast::error_code ec) {
    if (ec)
        return on_fail(ec, "ssl_handshake");

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req) {
            req.set(http::field::user_agent, "ibitcoin2waygrid");
        }));

    auto host = host_ + ':' + port_;

    // Perform the websocket handshake
    ws_.async_handshake(host, path_,
        beast::bind_front_handler(
            &WSSession::on_handshake,
            shared_from_this()));
}

void WSSession::on_handshake(beast::error_code ec) {
    if (ec)
        return on_fail(ec, "handshake");

    ws_.control_callback(
        [&] (websocket::frame_type kind, beast::string_view payload) {
            boost::ignore_unused(kind, payload);
            if (kind == websocket::frame_type::pong) {
                std::unique_lock lock(mutex_);
                last_pong_tp_ = std::chrono::steady_clock::now();
            }
        }
    );

    std::unique_lock lock(mutex_);
    connected_ = true;
    conn_condition_.notify_one();
}

bool WSSession::waitUtilConnected(std::chrono::seconds sec) {
    std::unique_lock lock(mutex_);
    if (ec_)
        return false;
    conn_condition_.wait_for(lock, sec);
    return connected_;
}

void WSSession::send(const std::string& data) {
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
    ws_.write(net::buffer(data));
}

void WSSession::read(std::string* out_data) {
    beast::flat_buffer buffer;
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
    ws_.read(buffer);
    *out_data = beast::buffers_to_string(buffer.data());
}

bool WSSession::async_read(std::function<void(beast::error_code ec, std::string&)> func) {
    {
        std::unique_lock lock(mutex_);
        if (async_read_called_)
            return false;
        async_read_called_ = true;
    }

    beast::get_lowest_layer(ws_).expires_never();
    ws_.async_read(
        buffer_,
        [func, this](beast::error_code ec, size_t) {
            {
                std::unique_lock lock(mutex_);
                async_read_called_ = false;
            }

            std::string str = beast::buffers_to_string(buffer_.data());
            buffer_.clear();
            func(ec, str);
        });
    return true;
}

void WSSession::on_fail(beast::error_code ec, char const* what) {
    auto msg = ec.message();
    LOG(error) << what << ": " << msg;

    std::unique_lock lock(mutex_);
    ec_ = ec;
    conn_condition_.notify_one();
}

void WSSession::ping() {
    ws_.ping({});
}

void WSSession::close() {
    ws_.close(websocket::close_code::normal);
}