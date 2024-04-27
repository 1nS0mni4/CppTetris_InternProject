#include "pch.h"
#include "ServerPacketHandler.h"

ServerPacketHandler::ServerPacketHandler() {
	Init();
}

ServerPacketHandler::~ServerPacketHandler() {

}

void ServerPacketHandler::HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size) {
	_func[(PacketType)packetID](session, packet, size);
}

void ServerPacketHandler::Init() {
	Register();
}

void ServerPacketHandler::Register() {
	_func[PacketType::TestPacket] = TestPacketHandler;
}

static void TestPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr)
		return;

	if (segment == nullptr)
		return;

	if (size == 0)
		return;

	TestPacket packet;
	if (size != packet.Read(segment))
		return;

	cout << "Session: " << session->GetSessionID() << " Sended: " << packet.data << '\n';

	return;
}