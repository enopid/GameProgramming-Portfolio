#pragma once

#include "../ServerCore/Protocol/CelesteProtocol.h"

#include <memory>
#include <unordered_map>
#include <deque>
#include <vector>

class ClientService;
class CRemotePlayer;
class ServerSession;

using ClientServiceRef = std::shared_ptr<ClientService>;
using ServerSessionRef = std::shared_ptr<ServerSession>;

struct RemotePlayerNetworkState
{
	int32 stageId = -1;
	PlayerStateData state;
};

class CNetworkManager
{
public:
	static CNetworkManager& Instance()
	{
		static CNetworkManager instance;
		return instance;
	}

	bool Initialize();
	bool Initialize(const std::wstring& serverIp, uint16 port);
	void Update(double deltaTime);
	void Release();
	void NotifyStageChanged(int32 stageId);

	bool IsConnected() const { return _connected; }
	uint64 GetMyPlayerId() const { return _myPlayerId; }
	int32 GetCurrentStageId() const { return _currentStageId; }
	double GetPingMs() const { return m_dRTT; }
	int32 GetSimulatedPingMs() const { return _simulatedPingMs; }
	const std::unordered_map<uint64, RemotePlayerNetworkState>& GetRemotePlayers() const { return _remotePlayers; }
	const std::unordered_map<uint64, CRemotePlayer*>& GetRemotePlayerVisuals() const { return _remotePlayerVisuals; }

	void OnConnected(const ServerSessionRef& session);
	void OnDisconnected();
	void HandlePacket(BYTE* buffer, int32 len);

private:
	CNetworkManager() = default;
	~CNetworkManager() = default;
	CNetworkManager(const CNetworkManager&) = delete;
	CNetworkManager& operator=(const CNetworkManager&) = delete;

	void SendCurrentStage();
	void SendLocalPlayerState();
	void SendPingPong();
	void SendPacket(const SendBufferRef& packet);
	void ProcessPacket(BYTE* buffer, int32 len);
	void FlushDelayedPackets();
	void CycleSimulatedPing();

	void HandleWelcome		(BYTE* buffer, int32 len);
	void HandlePlayerEntered(BYTE* buffer, int32 len);
	void HandlePlayerLeft	(BYTE* buffer, int32 len);
	void HandlePlayerState	(BYTE* buffer, int32 len);
	void HandlePingPong		(BYTE* buffer, int32 len);

	void UpdateRemotePlayerVisual(uint64 playerId, const PlayerStateData& state);
	void RemoveRemotePlayer(uint64 playerId);
	void ClearRemotePlayers();

private:
	struct DelayedOutgoingPacket
	{
		std::chrono::steady_clock::time_point sendAt;
		SendBufferRef packet;
	};

	struct DelayedIncomingPacket
	{
		std::chrono::steady_clock::time_point processAt;
		std::vector<BYTE> data;
	};

	ClientServiceRef _service;
	ServerSessionRef _session;
	bool _initialized = false;
	bool _connected = false;
	uint64 _myPlayerId = 0;
	int32 _currentStageId = -1;
	double _stateSendAccumulator = 0.0;
	uint32 _nextSequence			= 1;
	uint32 _nextPingPongSequence	= 1;
	std::unordered_map<uint64, RemotePlayerNetworkState>_remotePlayers;
	std::unordered_map<uint64, CRemotePlayer*>			_remotePlayerVisuals;
	std::deque<DelayedOutgoingPacket> _delayedOutgoingPackets;
	std::deque<DelayedIncomingPacket> _delayedIncomingPackets;
	int32 _simulatedPingMs = 0;

	double		m_dRTT = -1.0;
private:
	UniqueTimer	m_upPingPongTimer;
	UniqueTimer	m_upPlayerStateTimer;
};
