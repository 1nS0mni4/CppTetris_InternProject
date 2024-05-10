#include "pch.h"
#include "ClientPacketHandler.h"
#include "Packet.h"
#include "ServerSession.h"

extern atomic<bool> isLogined;
extern atomic<bool> isRunning;
extern mutex otherMtx;
extern unsigned char otherField[FIELD_LEN];
extern wchar_t otherName[NAME_LEN];
extern int otherCurX;
extern int otherCurY;
extern int otherRotation;
extern int otherCurPiece;
extern int otherScore;
extern int myWin;
extern int myLose;
extern int maxScore;

ClientPacketHandler::ClientPacketHandler() {
	Init();
}

ClientPacketHandler::~ClientPacketHandler() {

}

void ClientPacketHandler::HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size) {
	auto func = _func[(PacketType)packetID];
	if (func == nullptr) {
		DefaultPacketHandler(session, packet, size);
	}
	else
		func(session, packet, size);
}

void ClientPacketHandler::Init() {
	Register();
}

void ClientPacketHandler::Register() {
	_func[PacketType::Test] = TestPacketHandler;
	_func[PacketType::StC_MatchingResult] = StC_MatchingResultPacketHandler;
	_func[PacketType::StC_LoginResponse] = StC_LoginResponsePacketHandler;
	_func[PacketType::StC_AlreadyInUser] = StC_AlreadyInUserPacketHandler;
	_func[PacketType::StC_UserData] = StC_UserDataPacketHandler;
	_func[PacketType::StC_UserField] = StC_UserFieldPacketHandler;
	_func[PacketType::StC_UserScore] = StC_UserScorePacketHandler;
	_func[PacketType::StC_UserCurrentPiece] = StC_UserCurrentPiecePacketHandler;
	_func[PacketType::StC_ChallengerData] = StC_ChallengerDataPacketHandler;
	_func[PacketType::StC_UserLose] = StC_UserLosePacketHandler;
	_func[PacketType::StC_UserExit] = StC_UserExitPacketHandler;
	_func[PacketType::StC_GameStart] = StC_GameStartPacketHandler;
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
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_MatchingResultPacket packet;
	if (size != packet.Read(segment))
		return;


}

void StC_LoginResponsePacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_LoginResponsePacket packet;
	if (size != packet.Read(segment))
		return;

	s->isLogined = true;
}

void StC_AlreadyInUserPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_AlreadyInUserPacket packet;
	if (size != packet.Read(segment))
		return;

	memcpy(otherName, packet.name, NAME_LEN);
}

void StC_UserDataPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_UserDataPacket packet;
	if (size != packet.Read(segment))
		return;

	myWin = packet.win;
	myLose = packet.lose;
	maxScore = packet.maxScore;
}

void StC_UserFieldPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_UserFieldPacket packet;
	if (size != packet.Read(segment))
		return;

	lock_guard<mutex> guard(otherMtx);
	memcpy(otherField, packet.field, FIELD_LEN);
}

void StC_UserScorePacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_UserScorePacket packet;
	if (size != packet.Read(segment))
		return;

	lock_guard<mutex> guard(otherMtx);
	otherScore = packet.score;
}

void StC_UserCurrentPiecePacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_UserCurrentPiecePacket packet;
	if (size != packet.Read(segment))
		return;

	lock_guard<mutex> guard(otherMtx);
	if (otherCurPiece != packet.currentPiece) {
		otherCurPiece = packet.currentPiece;
		otherRotation = packet.rotation;
		otherCurX = packet.currentX;
		otherCurY = packet.currentY;
	}
}

void StC_ChallengerDataPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	StC_ChallengerDataPacket packet;
	if (size != packet.Read(segment))
		return;

	memcpy(otherName, packet.name, NAME_LEN);
}

void StC_UserLosePacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	s->isRunning = false;
}

void StC_UserExitPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	s->isRunning = false;
}

void StC_GameStartPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = dynamic_cast<ServerSession*>(session);
	if (s == nullptr)
		return;

	s->isRunning = true;
}


void DefaultPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ServerSession* s = (ServerSession*)session;

	cout << "[UnRegistered Packet Used] : SessionID(" << session->GetSessionID() << ")\n";
}