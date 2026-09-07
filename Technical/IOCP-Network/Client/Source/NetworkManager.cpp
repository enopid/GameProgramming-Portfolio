#include "pch.h"
#include "NetworkManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "TimeManager.h"

#include "../ServerCore/IocpCore.h"
#include "../ServerCore/NetAddress.h"
#include "../ServerCore/Protocol/NetworkSettings.h"
#include "Player.h"
#include "RemotePlayer.h"
#include "ServerSession.h"
#include "../ServerCore/Service.h"
#include "../ServerCore/SocketUtils.h"


namespace
{
	constexpr double PlayerStateSendInterval	= 1.0 / 20.0;
	constexpr double PingPongSendInterval		= 1.0;
	constexpr int32 MaxNetworkEventsPerFrame = 64;
}

bool CNetworkManager::Initialize()
{
	const NetworkSettings settings = LoadNetworkSettings();
	return Initialize(settings.serverIp, settings.port);
}

bool CNetworkManager::Initialize(const std::wstring& serverIp, uint16 port)
{
	if (_initialized)
		return true;

	SocketUtils::Init();
	auto iocpCore = std::make_shared<IocpCore>();
	_service = std::make_shared<ClientService>(
		NetAddress(serverIp, port), iocpCore,
		[this]() -> SessionRef
		{
			_session = std::make_shared<ServerSession>();
			return _session;
		}, 1);

	_initialized = _service->Start();
	if (!_initialized)
	{
		_service.reset();
		_session.reset();
		SocketUtils::Clear();
	}

	m_upPingPongTimer = CreateTimerWithDuration(PingPongSendInterval, true, [&]() {SendPingPong(); }, true);
	m_upPlayerStateTimer = CreateTimerWithDuration(PlayerStateSendInterval, true, [&]() {SendLocalPlayerState(); }, true);

	return _initialized;
}

void CNetworkManager::Update(double deltaTime)
{
	if (!_initialized || !_service)
		return;

	for (int32 i = 0; i < MaxNetworkEventsPerFrame; ++i)
	{
		if (!_service->GetIocpCore()->Dispatch(0))
			break;
	}

	FlushDelayedPackets();
	if (CInputManager::Instance().IsKeyState(VK_F8, EKeyActionState::Enter))
		CycleSimulatedPing();

	NotifyStageChanged(CStageManager::Instance().GetCurStage());
}

void CNetworkManager::Release()
{
	if (!_initialized)
		return;

	if (_session && _session->IsConnected())
		_session->Disconnect(L"Client shutdown");

	ClearRemotePlayers();
	_session.reset();
	_service.reset();
	_connected = false;
	_initialized = false;
	_myPlayerId = 0;
	_delayedOutgoingPackets.clear();
	_delayedIncomingPackets.clear();
	SocketUtils::Clear();
}

void CNetworkManager::NotifyStageChanged(int32 stageId)
{
	if (_currentStageId == stageId)
		return;
	_currentStageId = stageId;
	ClearRemotePlayers();
	SendCurrentStage();
}

void CNetworkManager::OnConnected(const ServerSessionRef& session)
{
	_session = session;
	_connected = true;
	PrintDebug(L"Network connected\n");
}

void CNetworkManager::OnDisconnected()
{
	_connected = false;
	_myPlayerId = 0;
	_delayedOutgoingPackets.clear();
	_delayedIncomingPackets.clear();
	ClearRemotePlayers();
	PrintDebug(L"Network disconnected\n");
}

void CNetworkManager::HandlePacket(BYTE* buffer, int32 len)
{
	if (buffer == nullptr || len < static_cast<int32>(sizeof(PacketHeader))) return;
	if (_simulatedPingMs > 0)
	{
		DelayedIncomingPacket delayedPacket;
		delayedPacket.processAt = std::chrono::steady_clock::now()
			+ std::chrono::milliseconds(_simulatedPingMs / 2);
		delayedPacket.data.assign(buffer, buffer + len);
		_delayedIncomingPackets.push_back(std::move(delayedPacket));
		return;
	}

	ProcessPacket(buffer, len);
}

void CNetworkManager::ProcessPacket(BYTE* buffer, int32 len)
{
	if (buffer == nullptr || len < static_cast<int32>(sizeof(PacketHeader))) return;
	const auto* header = reinterpret_cast<const PacketHeader*>(buffer);
	switch (static_cast<CelestePacketId>(header->id))
	{
	case CelestePacketId::SWelcome:			HandleWelcome		(buffer, len); break;
	case CelestePacketId::SPlayerEntered:	HandlePlayerEntered	(buffer, len); break;
	case CelestePacketId::SPlayerLeft:		HandlePlayerLeft	(buffer, len); break;
	case CelestePacketId::SPlayerState:		HandlePlayerState	(buffer, len); break;
	case CelestePacketId::SPingPong:		HandlePingPong		(buffer, len); break;
	default: break;
	}
}

void CNetworkManager::SendCurrentStage()
{
	if (!_connected || _myPlayerId == 0 || !_session)
		return;
	CStageChangedPacket packet;
	packet.stageId = _currentStageId;
	SendPacket(MakeCelestePacket(CelestePacketId::CStageChanged, packet));
}

void CNetworkManager::SendLocalPlayerState()
{
	if (!_connected || _myPlayerId == 0 || _currentStageId < 0 || !_session)
		return;

	auto* player = dynamic_cast<CPlayer*>(CObjectManager::Instance().Get_Player());
	if (player == nullptr) return;

	const pos<double> position	= player->GetNetworkPosition();
	const pos<double> velocity	= player->GetNetworkVelocity();
	const pos<double> acc		= player->GetNetworkAcc();
	const pos<int> direction	= player->GetDirection();

	CPlayerStatePacket packet;
	packet.state.posX			= position.first;
	packet.state.posY			= position.second;
	packet.state.velocityX		= static_cast<float>(velocity.first);
	packet.state.velocityY		= static_cast<float>(velocity.second);
	packet.state.accX			= static_cast<float>(acc.first);
	packet.state.accY			= static_cast<float>(acc.second);
	packet.state.sequence		= _nextSequence++;
	packet.state.iPlayerState	= static_cast<int8>(player->GetNetworkState());
	packet.state.facingX		= static_cast<int8>(direction.first);
	packet.state.facingY		= static_cast<int8>(direction.second);
	packet.state.RTT			= m_dRTT;

	SendPacket(MakeCelestePacket(CelestePacketId::CPlayerState, packet));
}

void CNetworkManager::SendPingPong()
{
	if (!_connected || _myPlayerId == 0 || !_session) return;
	PingPongPacket packet;
	packet.sequence = ++_nextPingPongSequence;
	packet.sendTime = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();
	SendPacket(MakeCelestePacket(CelestePacketId::CPingPong, packet));
}

void CNetworkManager::SendPacket(const SendBufferRef& packet)
{
	if (!_session || !_session->IsConnected() || !packet)
		return;

	if (_simulatedPingMs <= 0)
	{
		_session->Send(packet);
		return;
	}

	_delayedOutgoingPackets.push_back({
		std::chrono::steady_clock::now() + std::chrono::milliseconds(_simulatedPingMs / 2),
		packet
	});
}

void CNetworkManager::FlushDelayedPackets()
{
	const auto now = std::chrono::steady_clock::now();

	while (!_delayedOutgoingPackets.empty()
		&& (_simulatedPingMs <= 0 || _delayedOutgoingPackets.front().sendAt <= now))
	{
		auto delayedPacket = std::move(_delayedOutgoingPackets.front());
		_delayedOutgoingPackets.pop_front();
		if (_session && _session->IsConnected())
			_session->Send(delayedPacket.packet);
	}

	while (!_delayedIncomingPackets.empty()
		&& (_simulatedPingMs <= 0 || _delayedIncomingPackets.front().processAt <= now))
	{
		auto delayedPacket = std::move(_delayedIncomingPackets.front());
		_delayedIncomingPackets.pop_front();
		ProcessPacket(delayedPacket.data.data(), static_cast<int32>(delayedPacket.data.size()));
	}
}

void CNetworkManager::CycleSimulatedPing()
{
	switch (_simulatedPingMs)
	{
	case 0:   _simulatedPingMs = 100; break;
	case 100: _simulatedPingMs = 200; break;
	case 200: _simulatedPingMs = 400; break;
	default:  _simulatedPingMs = 0; break;
	}

	PrintDebug(L"Simulated ping: ", _simulatedPingMs, L" ms\n");
}

void CNetworkManager::HandleWelcome(BYTE* buffer, int32 len)
{
	SWelcomePacket packet;
	if (!ReadCelestePacket(buffer, len, packet)) return;
	_myPlayerId = packet.playerId;
	SendCurrentStage();
}

void CNetworkManager::HandlePlayerEntered(BYTE* buffer, int32 len)
{
	SPlayerEnteredPacket packet;
	if (!ReadCelestePacket(buffer, len, packet) || packet.playerId == _myPlayerId || packet.stageId != _currentStageId) return;
	_remotePlayers[packet.playerId] = { packet.stageId, packet.state };
	UpdateRemotePlayerVisual(packet.playerId, packet.state);
}

void CNetworkManager::HandlePlayerLeft(BYTE* buffer, int32 len)
{
	SPlayerLeftPacket packet;
	if (ReadCelestePacket(buffer, len, packet)) RemoveRemotePlayer(packet.playerId);
}

void CNetworkManager::HandlePlayerState(BYTE* buffer, int32 len)
{
	SPlayerStatePacket packet;
	if (!ReadCelestePacket(buffer, len, packet) || packet.playerId == _myPlayerId) return;
	if (packet.stageId != _currentStageId)
	{
		RemoveRemotePlayer(packet.playerId);
		return;
	}

	RemotePlayerNetworkState& remote = _remotePlayers[packet.playerId];
	if (remote.state.sequence != 0 && packet.state.sequence <= remote.state.sequence) return;
	remote.stageId = packet.stageId;
	remote.state = packet.state;
	UpdateRemotePlayerVisual(packet.playerId, packet.state);
}

void CNetworkManager::HandlePingPong(BYTE* buffer, int32 len)
{
	PingPongPacket packet;
	if (!ReadCelestePacket(buffer, len, packet)) return;
	if (packet.sequence != _nextPingPongSequence) return;

	int64_t nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();
	// Ping is displayed as the full round-trip time in milliseconds.
	m_dRTT = (nowUs - packet.sendTime) / 1000.0;
}

void CNetworkManager::UpdateRemotePlayerVisual(uint64 playerId, const PlayerStateData& state)
{
	auto& remotePlayer = _remotePlayerVisuals[playerId];
	if (!remotePlayer)
	{
		remotePlayer = new CRemotePlayer;
		CSceneManager::Instance().GetScene()->CreateObject(REMOTEPLAYER, remotePlayer);
	}
	remotePlayer->ApplyNetworkState(state);
}

void CNetworkManager::RemoveRemotePlayer(uint64 playerId)
{
	_remotePlayers.erase(playerId);

	auto it = _remotePlayerVisuals.find(playerId);
	if (it != _remotePlayerVisuals.end())
	{
		if (it->second != nullptr)
			it->second->SetDead(true);

		_remotePlayerVisuals.erase(it);
	}
}

void CNetworkManager::ClearRemotePlayers()
{
	_remotePlayers.clear();
	for (auto [_, pObject] : _remotePlayerVisuals) pObject->SetDead(true);
	_remotePlayerVisuals.clear();
}
