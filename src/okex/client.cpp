#include "client.h"

namespace okex {

    Client g_client;


    Client::Client() {
    }

    bool Client::start(const std::string& conf_file) {
        if (!settings_.load(conf_file))
            return false;

        auto& info = settings_.server_info;

        public_channel_ = std::make_shared<PublicChannel>(ioc_, false, info.host, info.port, info.public_path, settings_.socks_proxy);
        private_channel_ = std::make_shared<PrivateChannel>(ioc_, true, info.host, info.port, info.private_path, settings_.socks_proxy);
        rest_api_ = std::make_shared<RestApi>(ioc_, info.restapi_host, "443", settings_.socks_proxy);

        public_channel_->waitConnected();
        private_channel_->waitConnected();

        io_thread_.reset(new std::thread([&]() { ioc_.run(); }));


//        while (!g_user_data.balance_.inited || g_user_data.public_product_info_.data.empty() || g_user_data.public_trades_info_.trades_data.empty())
//            std::this_thread::sleep_for(std::chrono::seconds(1));
        return true;
    }



}