#include "public_channel.h"
#include "client.h"
#include "command.h"
#include "../utils/logger.h"

namespace okex {


    void PublicChannel::onConnected() {
        LOG(info) << "public channel connected.";

        auto req = Command::makeSubscribeInstrumentsChannel("SWAP");

        LOG(debug) << ">> subscribe instruments. " << req.data;

        this->sendCmd(std::move(req),
            [this](Command::Response& resp) {
                if (resp.code == 0)
                    LOG(debug) << "<< subscribe ok. " << resp.data;
                else
                    throw std::runtime_error("<< subscribe failed! " + resp.msg);
            }
        );

        for (auto& ticket : g_client.settings().tickets) {
            req = Command::makeSubscribeTickersChannel(ticket);

            LOG(debug) << ">> subscribe tickers. " << req.data;

            this->sendCmd(std::move(req),
                [this](Command::Response& resp) {
                    if (resp.code == 0)
                        LOG(debug) << "<< subscribe ok. " << resp.data;
                    else
                        throw std::runtime_error("<< subscribe failed! " + resp.msg);
                }
            );
        }

        req = Command::makeSubscribeStatusChannel();

        LOG(debug) << ">> subscribe status. " << req.data;

        this->sendCmd(std::move(req),
            [this](Command::Response& resp) {
                if (resp.code == 0)
                    LOG(debug) << "<< subscribe ok. " << resp.data;
                else
                    throw std::runtime_error("<< subscribe failed! " + resp.msg);
            }
        );
    }


}