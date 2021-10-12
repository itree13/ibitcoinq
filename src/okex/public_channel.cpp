#include "public_channel.h"
#include "command.h"
#include "../utils/logger.h"

extern std::string g_ticket;

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

    req = Command::makeSubscribeTradesChannel(g_ticket);

    LOG(debug) << ">> subscribe trades. " << req.data;

    this->sendCmd(std::move(req),
        [this](Command::Response& resp) {
            if (resp.code == 0)
                LOG(debug) << "<< subscribe ok. " << resp.data;
            else
                throw std::runtime_error("<< subscribe failed! " + resp.msg);
        }
    );

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
