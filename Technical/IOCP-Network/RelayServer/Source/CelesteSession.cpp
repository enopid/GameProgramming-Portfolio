#include "pch.h"
#include "CelesteSession.h"
#include "CelesteSessionManager.h"
#include "ServerPacketHandler.h"

void CelesteSession::OnConnected()
{
	GCelesteSessionManager.Add(GetCelesteSessionRef());

	SWelcomePacket packet;
	packet.playerId = _playerId;
	Send(MakeCelestePacket(CelestePacketId::SWelcome, packet));

	std::cout << "[Connected] player=" << _playerId << "\n";
}

void CelesteSession::OnDisconnected()
{
	std::cout << "[Disconnected] player=" << _playerId << " stage=" << _stageId << "\n";
	GCelesteSessionManager.Remove(GetCelesteSessionRef());
}

void CelesteSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	ServerPacketHandler::HandlePacket(GetCelesteSessionRef(), buffer, len);
}

void CelesteSession::OnSend(int32 len)
{
	UNREFERENCED_PARAMETER(len);
}

