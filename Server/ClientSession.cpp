#include "pch.h"
#include "ClientSession.h"
#include "PacketQueue.h"


void ClientSession::Disconnect() {
	Session::Disconnect();
	ClientSessionManager::GetInstance().RemoveSession(_sessionID);
}

void ClientSession::OnSend() {

}

int ClientSession::OnRecv(char* segment, int totalSize) {
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

void ClientSession::OnConnect() {
	
}

void ClientSession::OnDisconnect() {

}