#include "CorePch.h"
#include "ClientPacketHandler.h"
#include "Packet.h"

extern atomic<bool> isLogined;
extern atomic<bool> isRunning;
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

#define REGISTER_HANDLE(x) _func[PacketType::x] = x##PacketHandler;
#define BEGIN_FUNC(x)  void x##Handler(Session* session, char* segment, USHORT size) {\
if (session == nullptr || segment == nullptr)\
return;\
\
ServerSession* s = (ServerSession*)session;\
\
x packet;\
if (size != packet.Read(segment))\
return;

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
	REGISTER_HANDLE(StC_MatchingResult);
	REGISTER_HANDLE(StC_LoginResponse);
	REGISTER_HANDLE(StC_AlreadyInUser);
	REGISTER_HANDLE(StC_UserData);
	REGISTER_HANDLE(StC_UserField);
	REGISTER_HANDLE(StC_UserScore);
	REGISTER_HANDLE(StC_UserCurrentPiece);
	REGISTER_HANDLE(StC_ChallengerData);
	REGISTER_HANDLE(StC_UserLose);
	REGISTER_HANDLE(StC_UserExit);
	REGISTER_HANDLE(StC_GameStart);

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

	ServerSession* s = (ServerSession*)session;

	StC_MatchingResultPacket packet;
	if (size != packet.Read(segment))
		return;


}

BEGIN_FUNC(StC_LoginResponsePacket)
isLogined = true;
END_FUNC

BEGIN_FUNC(StC_AlreadyInUserPacket)
memcpy(otherName, packet.name, NAME_LEN);
END_FUNC

BEGIN_FUNC(StC_UserDataPacket)
myWin = packet.win;
myLose = packet.lose;
maxScore = packet.maxScore;
END_FUNC

BEGIN_FUNC(StC_UserFieldPacket)
memcpy(otherField, packet.field, FIELD_LEN);
END_FUNC

BEGIN_FUNC(StC_UserScorePacket)
otherScore = packet.score;
END_FUNC

BEGIN_FUNC(StC_UserCurrentPiecePacket)
otherCurPiece = packet.currentPiece;
otherRotation = packet.rotation;
otherCurX = packet.currentX;
otherCurY = packet.currentY;
END_FUNC

BEGIN_FUNC(StC_ChallengerDataPacket)
memcpy(otherName, packet.name, NAME_LEN);
END_FUNC

BEGIN_FUNC(StC_UserLosePacket)
isRunning = false;
END_FUNC

BEGIN_FUNC(StC_UserExitPacket)
isRunning = false;
END_FUNC

BEGIN_FUNC(StC_GameStartPacket)
isRunning = true;
END_FUNC