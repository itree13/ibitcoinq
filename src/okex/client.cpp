#include "client.h"

namespace okex {

    Client g_client;


    Client::Client() {
    }

    bool Client::start(const std::string& conf_file) {
        if (!settings_.load(conf_file))
            return false;

        public_channel_ = std::make_shared<PublicChannel>(ioc_, false, settings_.host, settings_.port, settings_.public_path, settings_.socks_proxy);
        private_channel_ = std::make_shared<PrivateChannel>(ioc_, true, settings_.host, settings_.port, settings_.private_path, settings_.socks_proxy);
        restapi_ = std::make_shared<RestApi>(ioc, restapi_host, "443", socks_proxy);

        g_public_channel->waitConnected();
        g_private_channel->waitConnected();

        io_thread_.reset(new std::thread([&]() { ioc_.run(); });


        while (!g_user_data.balance_.inited || g_user_data.public_product_info_.data.empty() || g_user_data.public_trades_info_.trades_data.empty())
            std::this_thread::sleep_for(std::chrono::seconds(1));
        return true;
    }



}