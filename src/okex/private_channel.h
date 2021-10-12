#pragma once

#include "channel.h"


class PrivateChannel : public Channel {
public:
    using Channel::Channel;

protected:
    void onConnected() override;

private:

};

extern std::shared_ptr<PrivateChannel> g_private_channel;
