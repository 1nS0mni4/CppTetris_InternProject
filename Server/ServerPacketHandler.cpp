#include "pch.h"
#include "ServerPacketHandler.h"
#include "RoomManager.h"
#include "DBManager.h"

#define SQL_ADD_NAME(query, name)\
	query += L" '";\
	query += name;\
	query += L"'";

#define SQL_ADD_PARAMETER(query, param)\
	query += L" ";\
	query += param;\

ServerPacketHandler::ServerPacketHandler() {
	Init();
}

ServerPacketHandler::~ServerPacketHandler() {

}

void ServerPacketHandler::HandlePacket(Session*  session, char* packet, USHORT packetID, USHORT size) {
	auto func = _func[packetID];
	if (func == nullptr) {
		DefaultPacketHandler(session, packet, size);
	}
	else {
		func(session, packet, size);
	}
}

void ServerPacketHandler::Init() {
	Register();
}

void ServerPacketHandler::Register() {
	_func[(USHORT)PacketType::Test] = TestPacketHandler;
	_func[(USHORT)PacketType::CtS_LoginRequest] = CtS_LoginRequestPacketHandler;
	_func[(USHORT)PacketType::CtS_MatchingRequest] = CtS_MatchingRequestPacketHandler;
	_func[(USHORT)PacketType::CtS_NotifyField] = CtS_NotifyFieldPacketHandler;
	_func[(USHORT)PacketType::CtS_NotifyScore] = CtS_NotifyScorePacketHandler;
	_func[(USHORT)PacketType::CtS_NotifyCurrentPiece] = CtS_NotifyCurrentPiecePacketHandler;
	_func[(USHORT)PacketType::CtS_NotifyLose] = CtS_NotifyLosePacketHandler;
}

void DefaultPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = (ClientSession*)session;
	
	cout << "[WARNING] UnRegistered Packet Used : SessionID (" << session->GetSessionID() << ")\n";
}

void TestPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = (ClientSession*)session;
	TestPacket packet;
	if (size != packet.Read(segment))
		return;

	cout << "Session ID: " << s->GetSessionID() << " / Data: " << packet.data << '\n';
}

void CtS_LoginRequestPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	CtS_LoginRequestPacket packet;
	if (size != packet.Read(segment))
		return;

	wmemcpy(s->name, packet.name, packet.nameLen);

	wstring query = DBManager::GetInstance().GetQuery(QueryType::UserLogin);
	SQL_ADD_NAME(query, s->name);

	DBManager::GetInstance().Execute((SQLWCHAR*)query.c_str());

	StC_LoginResponsePacket* p = new StC_LoginResponsePacket();
	p->result = 0;
	s->Send(p);
}

void CtS_MatchingRequestPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	RoomManager::GetInstance().EnterRoom(s);

	//cout << "Start Matchmaking: " << s->GetSessionID() << '\n';
}

void CtS_NotifyFieldPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	CtS_NotifyFieldPacket packet;
	if (size != packet.Read(segment))
		return;

	Room* room = s->_room;
	if (room == NULL)
		return;

	Session* other = room->GetOtherSession(s->GetSessionID());
	if (other == nullptr)
		return;

	StC_UserFieldPacket* p = new StC_UserFieldPacket();
	memcpy(p->field, packet.field, FIELD_LEN);
	other->Send(p);
}

void CtS_NotifyScorePacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	CtS_NotifyScorePacket packet;
	if (size != packet.Read(segment))
		return;

	Room* room = s->_room;
	if (room == NULL)
		return;

	room->UpdateScore(s->GetSessionID(), packet.score);
}

void CtS_NotifyCurrentPiecePacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	CtS_NotifyCurrentPiecePacket packet;
	if (size != packet.Read(segment))
		return;

	Room* room = s->_room;
	if (room == NULL)
		return;

	Session* other = room->GetOtherSession(s->GetSessionID());
	if (other == nullptr)
		return;

	//cout << "Piece: " << packet.currentPiece << " Rotation: " << packet.rotation << " currentX: " << packet.currentX << " currentY: " << packet.currentY << '\n';

	StC_UserCurrentPiecePacket* p = new StC_UserCurrentPiecePacket();
	p->currentPiece = packet.currentPiece;
	p->rotation = packet.rotation;
	p->currentX = packet.currentX;
	p->currentY = packet.currentY;

	other->Send(p);
}

void CtS_NotifyLosePacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	CtS_NotifyLosePacket packet;
	if (size != packet.Read(segment))
		return;

	Room* room = s->_room;
	ClientSession* other = (ClientSession*)room->GetOtherSession(s->GetSessionID());
	if (other == nullptr)
		return;

	StC_UserLosePacket* p = new StC_UserLosePacket();
	other->Send(p);

	wstring query = DBManager::GetQuery(QueryType::UpdateScore);
	SQL_ADD_NAME(query, s->name);
	SQL_ADD_PARAMETER(query, L',');
	SQL_ADD_PARAMETER(query, to_wstring(room->GetScore(s->GetSessionID())));
	DBManager::GetInstance().Execute(query);

	query = DBManager::GetQuery(QueryType::UpdateScore);
	SQL_ADD_NAME(query, other->name);
	SQL_ADD_PARAMETER(query, L',');
	SQL_ADD_PARAMETER(query, to_wstring(room->GetScore(other->GetSessionID())));
	DBManager::GetInstance().Execute(query);
}

bool Checker(char* segment, PacketType type) {
	USHORT packetID = *((USHORT*)&segment[0]);
	USHORT real = (USHORT)type;

	cout << "PacketID: " << packetID << ", Real One: " << real << '\n';
	return packetID == real;
}