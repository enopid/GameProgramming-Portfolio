#pragma once

#include "../SendBuffer.h"
#include "../Session.h"

#include <cstring>
#include <memory>
#include <type_traits>

enum class CelestePacketId : uint16
{
	SWelcome		= 1000,
	CStageChanged	= 1001,
	
	SPlayerEntered	= 1002,
	SPlayerLeft		= 1003,
	
	CPlayerState	= 1004,
	SPlayerState	= 1005,

	CPingPong		= 1006,
	SPingPong		= 1007,
};

#pragma pack(push, 1)

struct PlayerStateData
{
	int32 posX				= 0;
	int32 posY				= 0;
	float velocityX			= 0.f;
	float velocityY			= 0.f;
	float accX				= 0.f;
	float accY				= 0.f;
	uint32 sequence			= 0;
	int8 facingX			= 0;
	int8 facingY			= 0;
	int8 iPlayerState		= 0;
	double RTT				= 0.0;
};

struct SWelcomePacket
{
	uint64 playerId			= 0;
};

struct CStageChangedPacket
{
	int32 stageId			= -1;
};

struct SPlayerEnteredPacket
{
	uint64 playerId			= 0;
	int32 stageId			= -1;
	PlayerStateData state;
};

struct SPlayerLeftPacket
{
	uint64 playerId = 0;
	int32 stageId = -1;
};

struct CPlayerStatePacket
{
	PlayerStateData state;
};

struct SPlayerStatePacket
{
	uint64 playerId = 0;
	int32 stageId = -1;
	PlayerStateData state;
};

struct PingPongPacket
{
	uint32_t sequence;
	uint64_t sendTime;
};

#pragma pack(pop)

template<typename T>
SendBufferRef MakeCelestePacket(CelestePacketId packetId, const T& payload)
{
	static_assert(std::is_trivially_copyable_v<T>);

	constexpr size_t payloadSize = sizeof(T);
	constexpr size_t packetSize = sizeof(PacketHeader) + payloadSize;
	static_assert(packetSize <= UINT16_MAX);

	auto sendBuffer = std::make_shared<SendBuffer>(static_cast<int32>(packetSize));
	auto* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	header->size = static_cast<uint16>(packetSize);
	header->id = static_cast<uint16>(packetId);

	std::memcpy(header + 1, &payload, payloadSize);
	sendBuffer->Close(static_cast<uint32>(packetSize));
	return sendBuffer;
}

template<typename T>
bool ReadCelestePacket(BYTE* buffer, int32 len, T& payload)
{
	const int32 expectedSize = static_cast<int32>(sizeof(PacketHeader) + sizeof(T));
	if (buffer == nullptr || len != expectedSize)
		return false;

	const auto* header = reinterpret_cast<const PacketHeader*>(buffer);
	if (header->size != expectedSize)
		return false;

	std::memcpy(&payload, header + 1, sizeof(T));
	return true;
}
