#include "pch.h"
#include "ClientPacketHandler.h"

ClientPacketHandler::ClientPacketHandler() {
	Init();
}

ClientPacketHandler::~ClientPacketHandler() {

}

void ClientPacketHandler::HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size) {
	_func[(PacketType)packetID](session, packet, size);
}

void ClientPacketHandler::Init() {
	Register();
}

void ClientPacketHandler::Register() {
	_func[PacketType::Test] = TestPacketHandler;
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