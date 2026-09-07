#include "pch.h"
#include "CelesteSessionManager.h"

CelesteSessionManager GCelesteSessionManager;

void CelesteSessionManager::Add(const CelesteSessionRef& session)
{
	const uint64 playerId = _nextPlayerId.fetch_add(1);
	session->SetPlayerId(playerId);
	_sessions.emplace(playerId, session);
}

void CelesteSessionManager::Remove(const CelesteSessionRef& session)
{
	const uint64 playerId = session->GetPlayerId();
	const int32 stageId = session->GetStageId();

	_sessions.erase(playerId);

	if (stageId < 0)
		return;

	SPlayerLeftPacket packet;
	packet.playerId = playerId;
	packet.stageId = stageId;
	BroadcastToStage(stageId, MakeCelestePacket(CelestePacketId::SPlayerLeft, packet));
}

void CelesteSessionManager::ChangeStage(const CelesteSessionRef& session, int32 newStageId)
{
	const uint64 playerId	= session->GetPlayerId();
	const int32 oldStageId	= session->GetStageId();

	if (oldStageId == newStageId) return;

	if (oldStageId >= 0)
	{
		SPlayerLeftPacket leftPacket;
		leftPacket.playerId = playerId;
		leftPacket.stageId	= oldStageId;
		BroadcastToStage(oldStageId, MakeCelestePacket(CelestePacketId::SPlayerLeft, leftPacket), playerId);
	}

	session->SetStageId(newStageId);

	if (newStageId < 0)
		return;

	// First tell the entering client about players already in this stage.
	for (const auto& [otherPlayerId, otherSession] : _sessions)
	{
		if (otherPlayerId == playerId || otherSession->GetStageId() != newStageId)
			continue;

		SPlayerEnteredPacket existingPlayer;
		existingPlayer.playerId = otherPlayerId;
		existingPlayer.stageId = newStageId;
		existingPlayer.state = otherSession->GetPlayerState();
		session->Send(MakeCelestePacket(CelestePacketId::SPlayerEntered, existingPlayer));
	}

	// Then tell the existing clients about the newly entered player.
	SPlayerEnteredPacket enteredPacket;
	enteredPacket.playerId = playerId;
	enteredPacket.stageId = newStageId;
	enteredPacket.state = session->GetPlayerState();
	BroadcastToStage(newStageId, MakeCelestePacket(CelestePacketId::SPlayerEntered, enteredPacket), playerId);

	std::cout << "[Stage] player=" << playerId << " " << oldStageId << " -> " << newStageId << "\n";
}

void CelesteSessionManager::UpdatePlayerState(const CelesteSessionRef& session, const PlayerStateData& state)
{
	if (session->GetStageId() < 0)
		return;

	// Drop duplicated or older state updates after the first valid update.
	const uint32 previousSequence = session->GetPlayerState().sequence;
	if (previousSequence != 0 && state.sequence <= previousSequence)
		return;

	session->SetPlayerState(state);

	SPlayerStatePacket packet;
	packet.playerId = session->GetPlayerId();
	packet.stageId = session->GetStageId();
	packet.state = state;

	BroadcastToStage(
		session->GetStageId(),
		MakeCelestePacket(CelestePacketId::SPlayerState, packet),
		session->GetPlayerId());
}

void CelesteSessionManager::PingPong(const CelesteSessionRef& session, uint32_t sequence, uint64_t sendTime)
{
	const uint32 previousSequence = session->GetPingPongSequence();
	if (previousSequence != 0 && sequence <= previousSequence) return;
	session->SetPingPongSequence(sequence);
	PingPongPacket packet;
	packet.sendTime = sendTime;
	packet.sequence = sequence;
	session->Send(MakeCelestePacket(CelestePacketId::SPingPong, packet));
}

void CelesteSessionManager::BroadcastToStage(int32 stageId, const SendBufferRef& packet, uint64 exceptPlayerId)
{
	for (const auto& [playerId, session] : _sessions)
	{
		if (playerId == exceptPlayerId || session->GetStageId() != stageId)
			continue;

		session->Send(packet);
	}
}

