#include "pch.h"
#include "ServerSession.h"
#include "PacketQueue.h"

void ServerSession::Disconnect() {
	Session::Disconnect();
	ServerSessionManager::GetInstance().RemoveSession(_sessionID);
}

void ServerSession::OnSend() {

}

int ServerSession::OnRecv(char* segment, int totalSize) {
	int processed = 0;

	while (totalSize > PACKET_HEADER_SIZE) {
		USHORT packetID = ((USHORT*)segment)[0];
		USHORT size = ((USHORT*)segment)[1];

		PacketQueue::GetInstance().Push(this, segment, packetID, size);

		totalSize -= size;
		processed += size;
	}

	return processed;
}

void ServerSession::OnConnect() {

}

void ServerSession::OnDisconnect() {

}