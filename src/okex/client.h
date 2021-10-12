#include "settings.h"
#include "public_channel.h"
#include "private_channel.h"
#include "restapi.h"
#include <thread>

namespace okex {


    class Client {
    public:
        Client();

        bool start(const std::string& conf_file);

        const Settings& settings() const {
            return settings_;
        }

    private:
        Settings settings_;
        net::io_context ioc_;
        std::unique_ptr<std::thread> io_thread_;

        std::shared_ptr<PublicChannel> public_channel_;
        std::shared_ptr<PrivateChannel> private_channel_;
        std::shared_ptr<RestApi> rest_api_;
    };

    extern Client g_client;


}