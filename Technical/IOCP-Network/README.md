# IOCP Network

IOCP 릴레이 서버와 패킷 처리, 스냅샷 보간 및 데드 레커닝에 사용한 코드입니다.

## 포함 파일

| 파일 | 구분 |
|---|---|
| `Client/Include/NetworkManager.h` | Header |
| `Client/Include/RemotePlayer.h` | Header |
| `Client/Include/ServerSession.h` | Header |
| `Client/Source/NetworkManager.cpp` | Source |
| `Client/Source/RemotePlayer.cpp` | Source |
| `Client/Source/ServerSession.cpp` | Source |
| `Protocol/Include/CelesteProtocol.h` | Header |
| `Protocol/Include/NetworkSettings.h` | Header |
| `RelayServer/Include/CelesteSession.h` | Header |
| `RelayServer/Include/CelesteSessionManager.h` | Header |
| `RelayServer/Include/ServerPacketHandler.h` | Header |
| `RelayServer/Source/CelesteSession.cpp` | Source |
| `RelayServer/Source/CelesteSessionManager.cpp` | Source |
| `RelayServer/Source/ServerPacketHandler.cpp` | Source |
| `ServerCore/Include/BufferReader.h` | Header |
| `ServerCore/Include/BufferWriter.h` | Header |
| `ServerCore/Include/IocpCore.h` | Header |
| `ServerCore/Include/IocpEvent.h` | Header |
| `ServerCore/Include/Listener.h` | Header |
| `ServerCore/Include/NetAddress.h` | Header |
| `ServerCore/Include/RecvBuffer.h` | Header |
| `ServerCore/Include/SendBuffer.h` | Header |
| `ServerCore/Include/Service.h` | Header |
| `ServerCore/Include/Session.h` | Header |
| `ServerCore/Include/SocketUtils.h` | Header |
| `ServerCore/Source/BufferReader.cpp` | Source |
| `ServerCore/Source/BufferWriter.cpp` | Source |
| `ServerCore/Source/IocpCore.cpp` | Source |
| `ServerCore/Source/IocpEvent.cpp` | Source |
| `ServerCore/Source/Listener.cpp` | Source |
| `ServerCore/Source/NetAddress.cpp` | Source |
| `ServerCore/Source/RecvBuffer.cpp` | Source |
| `ServerCore/Source/SendBuffer.cpp` | Source |
| `ServerCore/Source/Service.cpp` | Source |
| `ServerCore/Source/Session.cpp` | Source |
| `ServerCore/Source/SocketUtils.cpp` | Source |

> 전체 프로젝트의 빌드 의존성은 포함하지 않았으며, 구현 흐름을 확인하는 데 필요한 범위까지만 선별했습니다.
