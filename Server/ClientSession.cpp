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
	char* buffer = segment;
	while (totalSize > PACKET_HEADER_SIZE) {
		USHORT packetID = *((USHORT*)&(segment[0]));
		USHORT size = *((USHORT*)&(segment[2]));
		
		if (size > totalSize)
			break;

		PacketQueue::GetInstance().Push(this, segment, packetID, size);

		segment = segment + size;
		totalSize -= size;
		processed += size;
	}

	return processed;
}

void ClientSession::OnConnect() {
	
}

void ClientSession::OnDisconnect() {

}