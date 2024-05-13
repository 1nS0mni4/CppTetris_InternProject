#include "pch.h"
#include "ClientSession.h"
#include "PacketQueue.h"
#include "ServerPacketHandler.h"

void ClientSession::OnSend() {

}

int ClientSession::OnRecv(char* segment, int totalSize) {
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

void ClientSession::OnConnect() {
	
}

void ClientSession::OnDisconnect() {
	if (_room == 0)
		return;

	_room->Exit(this);
}

void ClientSession::Disconnect() {
	ClientSessionManager::GetInstance().RemoveSession(_sessionID);
	Session::Disconnect();
}