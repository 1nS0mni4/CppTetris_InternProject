#include "CorePch.h"
#include "ClientPacketHandler.h"
#include "Packet.h"

#define REGISTER_HANDLE(x) _func[PacketType::x] = x##PacketHandler;
#define BEGIN_FUNC(x)void x##PacketHandler(Session* session, char* segment, USHORT size) {
#define END_FUNC	}

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
	REGISTER_HANDLE(Test);
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

void StC_MatchingResultPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr || size == 0)
		return;

	StC_MatchingResultPacket packet;
	if (size != packet.Read(segment))
		return;



}