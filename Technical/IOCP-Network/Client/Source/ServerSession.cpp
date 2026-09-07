#include "pch.h"
#include "ServerSession.h"
#include "NetworkManager.h"

void ServerSession::OnConnected()
{
	CNetworkManager::Instance().OnConnected(std::static_pointer_cast<ServerSession>(shared_from_this()));
}

void ServerSession::OnDisconnected()
{
	CNetworkManager::Instance().OnDisconnected();
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	CNetworkManager::Instance().HandlePacket(buffer, len);
}

void ServerSession::OnSend(int32 len)
{
	UNREFERENCED_PARAMETER(len);
}
