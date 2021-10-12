#pragma once

#include "channel.h"


class PublicChannel : public Channel {
public:
    using Channel::Channel;

protected:
    void onConnected() override;









};


extern std::shared_ptr<PublicChannel> g_public_channel;
