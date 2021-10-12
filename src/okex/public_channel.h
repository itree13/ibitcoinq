#pragma once

#include "channel.h"

namespace okex {


    class PublicChannel : public Channel {
    public:
        using Channel::Channel;

    protected:
        void onConnected() override;

    };


    extern std::shared_ptr<PublicChannel> g_public_channel;


}