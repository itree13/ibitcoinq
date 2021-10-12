#include "private_channel.h"
#include "command.h"
#include "restapi.h"
#include "../utils/logger.h"

extern std::string g_api_key;
extern std::string g_passphrase;
extern std::string g_secret;

void PrivateChannel::onConnected() {
    /*auto req = Command::makeSubscriBalanceAndPositionsChannel();

    LOG(debug) << ">> subscribe bal&pos. " << req.data;

    this->sendCmd(std::move(req),
        [this](Command::Response& resp) {
            if (resp.code == 0)
                LOG(debug) << "<< subscribe ok. " << resp.data;
            else
                throw std::runtime_error("<< subscribe failed! " + resp.msg);
        }
    );*/

    auto req = Command::makeSubscribeAccountChannel();
    LOG(debug) << ">> subscribe account. " << req.data;
    this->sendCmd(std::move(req),
        [this](Command::Response& resp) {
            if (resp.code == 0)
                LOG(debug) << "<< subscribe ok. " << resp.data;
            else
                throw std::runtime_error("<< subscribe failed! " + resp.msg);
        }
    );

    req = Command::makeSubscribePositionsChannel();
    LOG(debug) << ">> subscribe positions. " << req.data;
    this->sendCmd(std::move(req),
        [this](Command::Response& resp) {
            if (resp.code == 0)
                LOG(debug) << "<< subscribe ok. " << resp.data;
            else
                throw std::runtime_error("<< subscribe failed! " + resp.msg);
        }
    );

    req = Command::makeSubscribeOrdersChannel();
    LOG(debug) << ">> subscribe. " << req.data;
    this->sendCmd(std::move(req),
        [this](Command::Response& resp) {
            if (resp.code == 0)
                LOG(debug) << "<< subscribe ok. " << resp.data;
            else
                throw std::runtime_error("<< subscribe failed! " + resp.msg);
        }
    );

    {
        g_user_data.lock();
        auto scoped_exit = make_scope_exit([] { g_user_data.unlock(); });
        if (!g_user_data.grid_strategy_.grids.empty()) {
            g_user_data.grid_strategy_.dirty = true;
        }
    }
}
