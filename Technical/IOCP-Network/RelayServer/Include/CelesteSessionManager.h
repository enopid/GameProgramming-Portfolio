#pragma once

#include "CelesteSession.h"

class CelesteSessionManager
{
public:
	void Add(const CelesteSessionRef& session);
	void Remove(const CelesteSessionRef& session);

	void ChangeStage		(const CelesteSessionRef& session, int32 newStageId);
	void UpdatePlayerState	(const CelesteSessionRef& session, const PlayerStateData& state);
	void PingPong			(const CelesteSessionRef& session, uint32_t sequence, uint64_t sendTime);

private:
	void BroadcastToStage(int32 stageId, const SendBufferRef& packet, uint64 exceptPlayerId = 0);

private:
	std::unordered_map<uint64, CelesteSessionRef> _sessions;
	std::atomic<uint64> _nextPlayerId = 1;
};

extern CelesteSessionManager GCelesteSessionManager;

