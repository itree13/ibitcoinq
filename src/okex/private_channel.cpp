#include "private_channel.h"
#include "client.h"
#include "command.h"
#include "../trades_manager.h"
#include "../utils/logger.h"

namespace okex {


    void PrivateChannel::onConnected() {
        LOG(info) << "private channel connected.";

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

        g_trades_man.markDataDirty();
    }


}