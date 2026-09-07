#pragma once

#include "CelesteSession.h"

class ServerPacketHandler
{
public:
	static void HandlePacket(const CelesteSessionRef& session, BYTE* buffer, int32 len);

private:
	static void HandleStageChanged(const CelesteSessionRef& session, BYTE* buffer, int32 len);
	static void HandlePlayerState	(const CelesteSessionRef& session, BYTE* buffer, int32 len);
	static void HandlePingPong		(const CelesteSessionRef& session, BYTE* buffer, int32 len);
};

