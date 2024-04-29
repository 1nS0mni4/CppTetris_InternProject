#include "pch.h"
#include "ClientSession.h"
#include "PacketQueue.h"
#include "ServerPacketHandler.h"




void ClientSession::OnSend() {

}

int ClientSession::OnRecv(char* segment, int totalSize) {
	int processed = 0;
	char* buffer = segment;

	while (totalSize > PACKET_HEADER_SIZE) {
		volatile USHORT packetID = *((USHORT*)&(buffer[0]));
		volatile USHORT size = *((USHORT*)&(buffer[2]));
		
		if (size > totalSize)
			break;

		PacketQueue::GetInstance().Push(this, buffer, packetID, size);

		buffer += size;
		totalSize -= size;
		processed += size;
	}

	return processed;
}

void ClientSession::OnConnect() {
	
}

void ClientSession::OnDisconnect() {
	if (_room)
		_room->Exit(this);
}

void ClientSession::Disconnect() {
	ClientSessionManager::GetInstance().RemoveSession(_sessionID);
	Session::Disconnect();
}