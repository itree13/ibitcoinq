#pragma once

#include "../type.h"

namespace okex {

//实盘API交易地址如下：

#define REST_API_HOST               "www.okex.com"
#define WSS_HOST                    "ws.okex.com"
#define WSS_PORT                    "8443"
#define WSS_PUBLIC_CHANNEL          "/ws/v5/public"
#define WSS_PRIVATE_CHANNEL         "/ws/v5/private"

// AWS 地址如下：
#define AWS_REST_API_HOST           "aws.okex.com"
#define AWS_WSS_HOST                "wsaws.okex.com"
#define AWS_WSS_PORT                "8443"
#define AWS_WSS_PUBLIC_CHANNEL      "/ws/v5/public"
#define AWS_WSS_PRIVATE_CHANNEL     "/ws/v5/private"

// 模拟盘API交易地址如下：
#define SIMU_REST_API_HOST          "www.okex.com"
#define SIMU_WSS_HOST               "wspap.okex.com"
#define SIMU_WSS_PORT               "8443"
#define SIMU_WSS_PUBLIC_CHANNEL     "/ws/v5/public?brokerId=9999"
#define SIMU_WSS_PRIVATE_CHANNEL    "/ws/v5/private?brokerId=9999"


    struct Settings {
        std::string api_key;
        std::string passphrase;
        std::string secret;
        std::string ticket;
        std::string enviorment;
        std::string socks_proxy;

        struct ServerInfo {
            bool is_simu;
            std::string host;
            std::string port;
            std::string private_path;
            std::string public_path;
            std::string restapi_host;
        } server_info;

        bool load(const std::string& filepath);
    };

    extern Settings g_settings;


}