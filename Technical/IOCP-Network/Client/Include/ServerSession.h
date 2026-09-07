#pragma once

#include "../ServerCore/Session.h"

class ServerSession final : public PacketSession
{
protected:
	void OnConnected() override;
	void OnDisconnected() override;
	void OnRecvPacket(BYTE* buffer, int32 len) override;
	void OnSend(int32 len) override;
};
