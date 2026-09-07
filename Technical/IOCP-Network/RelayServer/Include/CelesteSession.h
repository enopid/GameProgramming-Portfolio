#pragma once

#include "../ServerCore/Protocol/CelesteProtocol.h"
#include "Session.h"

class CelesteSession;
using CelesteSessionRef = std::shared_ptr<CelesteSession>;

class CelesteSession final : public PacketSession
{
public:
	uint64	GetPlayerId()			const { return _playerId; }
	int32	GetStageId()			const { return _stageId; }
	int32_t GetPingPongSequence()	const { return _pingpongSequence; }
	const PlayerStateData& GetPlayerState() const { return _state; }

	void SetPlayerId		(uint64 playerId)				{ _playerId = playerId; }
	void SetStageId			(int32 stageId)					{ _stageId = stageId; }
	void SetPingPongSequence(int32 pingpongSequence)		{ _pingpongSequence = pingpongSequence; }
	void SetPlayerState	(const PlayerStateData& state)		{ _state = state; }

	CelesteSessionRef GetCelesteSessionRef()
	{
		return std::static_pointer_cast<CelesteSession>(shared_from_this());
	}

protected:
	void OnConnected() override;
	void OnDisconnected() override;
	void OnRecvPacket(BYTE* buffer, int32 len) override;
	void OnSend(int32 len) override;

private:
	uint64 _playerId		= 0;
	int32 _stageId			= -1;
	int32 _pingpongSequence = 0;
	PlayerStateData _state;
};
