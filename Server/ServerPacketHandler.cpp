#include "pch.h"
#include "ServerPacketHandler.h"

ServerPacketHandler::ServerPacketHandler() {

}

ServerPacketHandler::~ServerPacketHandler() {

}

int ServerPacketHandler::HandlePacket(Session* session, Packet* packet, USHORT packetID) {
	return -1;
}

