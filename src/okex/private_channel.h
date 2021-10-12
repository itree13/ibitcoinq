#pragma once

#include "channel.h"

namespace okex {

    class PrivateChannel : public Channel {
    public:
        using Channel::Channel;

    protected:
        void onConnected() override;

    private:

    };


}