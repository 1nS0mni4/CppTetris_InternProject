#include "pch.h"
#include "ServerPacketHandler.h"
#include "RoomManager.h"

#define BEGIN_HANDLER(x) void x ## Handler(Session* session, char* segment, USHORT size) {\
if (session == nullptr || segment == nullptr)\
return;\
\
ClientSession* s = (ClientSession*)session;\
\
x packet;\
if (size != packet.Read(segment))\
return;

#define END_HANDLER	}

#define REGISTER_HANDLER(x) _func[PacketType::x] = x ## PacketHandler

ServerPacketHandler::ServerPacketHandler() {
	Init();
}

ServerPacketHandler::~ServerPacketHandler() {

}

void ServerPacketHandler::HandlePacket(Session*  session, char* packet, USHORT packetID, USHORT size) {
	_func[(PacketType)packetID](session, packet, size);
}

void ServerPacketHandler::Init() {
	Register();
}

void ServerPacketHandler::Register() {
	_func[PacketType::CtS_LoginRequest] = CtS_LoginRequestPacketHandler;
	_func[PacketType::CtS_MatchingRequest] = CtS_MatchingRequestPacketHandler;
	_func[PacketType::CtS_NotifyField] = CtS_NotifyFieldPacketHandler;
	_func[PacketType::CtS_NotifyScore] = CtS_NotifyScorePacketHandler;
	_func[PacketType::CtS_NotifyCurrentPiece] = CtS_NotifyCurrentPiecePacketHandler;
	_func[PacketType::CtS_NotifyLose] = CtS_NotifyLosePacketHandler;
}

void CtS_LoginRequestPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	CtS_LoginRequestPacket packet;
	

	StC_LoginResponsePacket* p = new StC_LoginResponsePacket();
	p->result = 0;
	s->Send(p);

	cout << "Sended Response to: " << s->GetSessionID() << '\n';
}

//TODO: DB에서 이름 확인해서 있으면 데이터 전송 없으면 생성 후 데이터 전송(디폴트)

void CtS_MatchingRequestPacketHandler(Session* session, char* segment, USHORT size) {
	if (session == nullptr || segment == nullptr)
		return;

	ClientSession* s = dynamic_cast<ClientSession*>(session);
	if (s == nullptr)
		return;

	RoomManager::GetInstance().EnterRoom(s);

	cout << "Start Matchmaking: " << s->GetSessionID() << '\n';
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
	Session* other = room->GetOtherSession(s->GetSessionID());
	if (other == nullptr)
		return;

	StC_UserScorePacket* p = new StC_UserScorePacket();
	p->score = packet.score;
	other->Send(p);
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
	Session* other = room->GetOtherSession(s->GetSessionID());
	if (other == nullptr)
		return;

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
	Session* other = room->GetOtherSession(s->GetSessionID());
	if (other == nullptr)
		return;

	StC_UserLosePacket* p = new StC_UserLosePacket();
	other->Send(p);
}