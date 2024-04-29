#include "pch.h"
#include "ClientPacketHandler.h"

extern char userName[50];
extern int oScore;
extern char* oField;
extern bool isLogin;
extern int myWin;
extern int myLose;
extern int myMaxScore;

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

	if (packet.result != 0) {
		cout << "Error!\n";
		return;
	}

	isLogin = true;
}

void StC_UserDataHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr)
		return;

	if (segment == nullptr)
		return;

	if (size == 0)
		return;

	StC_UserDataPacket packet;
	if (size != packet.Read(segment))
		return;

	myWin = packet.win;
	myLose = packet.lose;
	myMaxScore = packet.maxScore;
}

void StC_UserFieldHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr)
		return;

	if (segment == nullptr)
		return;

	if (size == 0)
		return;

	StC_UserFieldPacket packet;
	if (size != packet.Read(segment))
		return;

	strcpy_s((char*)oField, 216, packet.field);
}

void StC_UserScoreHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr)
		return;

	if (segment == nullptr)
		return;

	if (size == 0)
		return;

	StC_UserScorePacket packet;
	if (size != packet.Read(segment))
		return;

	oScore = packet.score;
}