#include "pch.h"
#include "ServerPacketHandler.h"
#include "CelesteSessionManager.h"

void ServerPacketHandler::HandlePacket(const CelesteSessionRef& session, BYTE* buffer, int32 len)
{
	if (buffer == nullptr || len < static_cast<int32>(sizeof(PacketHeader)))
	{
		session->Disconnect(L"Invalid packet header");
		return;
	}

	const auto* header = reinterpret_cast<const PacketHeader*>(buffer);
	switch (static_cast<CelestePacketId>(header->id))
	{
	case CelestePacketId::CStageChanged:
		HandleStageChanged(session, buffer, len);
		break;
	case CelestePacketId::CPlayerState:
		HandlePlayerState(session, buffer, len);
		break;
	case CelestePacketId::CPingPong:
		HandlePingPong(session, buffer, len);
		break;
	default:
		session->Disconnect(L"Unknown packet id");
		break;
	}
}

void ServerPacketHandler::HandleStageChanged(const CelesteSessionRef& session, BYTE* buffer, int32 len)
{
	CStageChangedPacket packet;
	if (!ReadCelestePacket(buffer, len, packet))
	{
		session->Disconnect(L"Invalid stage packet");
		return;
	}

	GCelesteSessionManager.ChangeStage(session, packet.stageId);
}

void ServerPacketHandler::HandlePlayerState(const CelesteSessionRef& session, BYTE* buffer, int32 len)
{
	CPlayerStatePacket packet;
	if (!ReadCelestePacket(buffer, len, packet))
	{
		session->Disconnect(L"Invalid player state packet");
		return;
	}

	GCelesteSessionManager.UpdatePlayerState(session, packet.state);
}

void ServerPacketHandler::HandlePingPong(const CelesteSessionRef& session, BYTE* buffer, int32 len)
{
	PingPongPacket packet;
	if (!ReadCelestePacket(buffer, len, packet))
	{
		session->Disconnect(L"Invalid pingpong packet");
		return;
	}

	GCelesteSessionManager.PingPong(session, packet.sequence, packet.sendTime);
}

