#include "ClientPacketHandler.h"
#include "Packet.h"

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
	_func[PacketType::TestPacket] = TestPacketHandler;
	_func[PacketType::StC_LoginResponse] = StC_LoginResponseHandler;

}

void TestPacketHandler(Session* session, char* segment, USHORT size) {
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

void StC_LoginResponseHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr)
		return;

	if (segment == nullptr)
		return;

	if (size == 0)
		return;

	StC_LoginResponsePacket packet;
	if (size != packet.Read(segment))
		return;

	if (packet.result == 0) {
		
		PostQuitMessage(0);
	}
		

	return;
}

void StC_PersonalDataHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr)
		return;

	if (segment == nullptr)
		return;

	if (size == 0)
		return;

	StC_LoginResponsePacket packet;
	if (size != packet.Read(segment))
		return;

	if (packet.result == 0)
		PostQuitMessage(0);

	return;
}