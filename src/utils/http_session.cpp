#include "http_session.h"
#include "logger.h"
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/strand.hpp>
#include "socks/handshake.hpp"

HttpSession::HttpSession(net::io_context& ioc, ssl::context& ctx,
    const std::string& host, const std::string& port)
: resolver_(net::make_strand(ioc)),
stream_(net::make_strand(ioc), ctx),
host_(host),
port_(port) {

}

void HttpSession::start() {
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
                &HttpSession::on_socks_proxy_resolve,
                shared_from_this()));
        return;
    }

    // Look up the domain name
    resolver_.async_resolve(
        host_,
        port_,
        beast::bind_front_handler(
            &HttpSession::on_resolve,
            shared_from_this()));
}

void HttpSession::on_socks_proxy_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec)
        return on_fail(ec, "resolve");

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));
    beast::get_lowest_layer(stream_).async_connect(
        results,
        beast::bind_front_handler(
            &HttpSession::on_socks_proxy_connect,
            shared_from_this()));
}

void HttpSession::on_socks_proxy_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec)
        return on_fail(ec, "proxy_connect");

    if (socks_version_ == 4)
        socks::async_handshake_v4(
            beast::get_lowest_layer(stream_),
            host_,
            static_cast<unsigned short>(std::atoi(port_.c_str())),
            std::string(socks_url_.username()),
            beast::bind_front_handler(
                &HttpSession::on_socks_proxy_handshake,
                shared_from_this()));
    else
        socks::async_handshake_v5(
            beast::get_lowest_layer(stream_),
            host_,
            static_cast<unsigned short>(std::atoi(port_.c_str())),
            std::string(socks_url_.username()),
            std::string(socks_url_.password()),
            true,
            beast::bind_front_handler(
                &HttpSession::on_socks_proxy_handshake,
                shared_from_this()));
}

void HttpSession::on_socks_proxy_handshake(beast::error_code ec) {
    if (ec)
        return on_fail(ec, "proxy_handshake");

    on_connect(ec, tcp::resolver::results_type::endpoint_type());
}

void HttpSession::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec)
        return on_fail(ec, "resolve");

    // Set a timeout on the operation
    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(stream_).async_connect(
        results,
        beast::bind_front_handler(
            &HttpSession::on_connect,
            shared_from_this()));
}

void HttpSession::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec)
        return on_fail(ec, "connect");

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    if (!SSL_set_tlsext_host_name(stream_.native_handle(), host_.c_str())) {
        beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
        std::cerr << ec.message() << "\n";
        return;
    }

    // Perform the SSL handshake
    stream_.async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &HttpSession::on_ssl_handshake,
            shared_from_this()));
}

void HttpSession::on_ssl_handshake(beast::error_code ec) {
    if (ec)
        return on_fail(ec, "ssl_handshake");

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    std::unique_lock lock(mutex_);
    connected_ = true;
    conn_condition_.notify_one();
}

bool HttpSession::waitUtilConnected(std::chrono::seconds sec) {
    std::unique_lock lock(mutex_);
    if (ec_)
        return false;
    conn_condition_.wait_for(lock, sec);
    return connected_;
}

void HttpSession::send(http::request<http::string_body>& req) {
    http::write(stream_, req);
}

bool HttpSession::read(http::response<http::string_body>& resp) {
    try {
        beast::flat_buffer buffer;
        http::read(stream_, buffer, resp);
        return true;
    } catch (...) {
    }
    return false;
}

void HttpSession::on_fail(beast::error_code ec, char const* what) {
    auto msg = ec.message();
    LOG(error) << what << ": " << msg;

    std::unique_lock lock(mutex_);
    ec_ = ec;
    conn_condition_.notify_one();
}