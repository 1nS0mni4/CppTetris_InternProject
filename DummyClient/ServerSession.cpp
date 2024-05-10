#include "pch.h"
#include "ServerSession.h"
#include "PacketQueue.h"

void ServerSession::OnSend() {

}

int ServerSession::OnRecv(char* segment, int totalSize) {
	int processed = 0;

	while (totalSize >= PACKET_HEADER_SIZE) {
		USHORT packetID = *((USHORT*)&(segment[0]));
		USHORT size = *((USHORT*)&(segment[2]));

		if (size > totalSize)
			break;

		PacketQueue::GetInstance().Push(this, segment, packetID, size);

		segment += size;
		totalSize -= size;
		processed += size;
	}

	return processed;
}

void ServerSession::OnConnect() {

}

void ServerSession::OnDisconnect() {

}

void ServerSession::Disconnect() {
	ServerSessionManager::GetInstance().RemoveSession(_sessionID);
	Session::Disconnect();
}