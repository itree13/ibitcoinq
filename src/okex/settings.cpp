#include "settings.h"
#include "../utils/logger.h"
#include "../utils/json.h"
#include <boost/dll.hpp>

namespace okex {


    bool Settings::load(const std::string& conf_file) {
        auto setting_filename = (boost::dll::program_location().parent_path() / conf_file).string();
        try {
            std::ifstream in(setting_filename);
            if (!in.is_open()) {
                LOG(error) << "open " << setting_filename << " failed!";
                return -1;
            }

            std::string json_content((std::istreambuf_iterator<char>(in)),
                std::istreambuf_iterator<char>());

            rapidjson::Document doc;
            doc.Parse<0>(json_content.data(), json_content.size());

            this->enviorment = doc["enviorment"].GetString();
            this->api_key = doc["api_key"].GetString();
            this->passphrase = doc["passphrase"].GetString();
            this->secret = doc["secret"].GetString();
            this->ticket = doc["ticket"].GetString();

            if (this->enviorment.empty() || this->api_key.empty() || this->secret.empty()) {
                LOG(error) << "missing api_key or secret!";
                return false;
            }

            if (this->ticket.empty()) {
                LOG(error) << "missing ticket!";
                return false;
            }

            if (doc.HasMember("socks_proxy"))
                this->socks_proxy = doc["socks_proxy"].GetString();

            if (this->enviorment == "simu") {
                this->server_info.is_simu = true;
                this->server_info.host = SIMU_WSS_HOST;
                this->server_info.port = SIMU_WSS_PORT;
                this->server_info.private_path = SIMU_WSS_PRIVATE_CHANNEL;
                this->server_info.public_path = SIMU_WSS_PUBLIC_CHANNEL;
                this->server_info.restapi_host = SIMU_REST_API_HOST;
            } else if (this->enviorment == "product") {
                this->server_info.is_simu = false;
                this->server_info.host = WSS_HOST;
                this->server_info.port = WSS_PORT;
                this->server_info.private_path = WSS_PRIVATE_CHANNEL;
                this->server_info.public_path = WSS_PUBLIC_CHANNEL;
                this->server_info.restapi_host = REST_API_HOST;
            } else if (this->enviorment == "aws") {
                this->server_info.is_simu = false;
                this->server_info.host = AWS_WSS_HOST;
                this->server_info.port = AWS_WSS_PORT;
                this->server_info.private_path = AWS_WSS_PRIVATE_CHANNEL;
                this->server_info.public_path = AWS_WSS_PUBLIC_CHANNEL;
                this->server_info.restapi_host = AWS_REST_API_HOST;
            } else {
                LOG(error) << "invalid enviorenment setting! " << this->enviorment << ". (simu, product, aws)";
                return false;
            }
        } catch (const std::exception& e) {
            LOG(error) << "pase " << setting_filename << " failed!" << e.what();
            return false;
        }
        return true;
    }


}