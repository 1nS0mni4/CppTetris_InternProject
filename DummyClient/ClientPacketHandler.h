#pragma once
#include "pch.h"
#include "Packet.h"
#include "ServerSession.h"

typedef void (*HandlerFunc)(Session* session, char* segment, USHORT size);

void TestPacketHandler(Session* session, char* segment, USHORT size);
void StC_MatchingResultPacketHandler(Session* session, char* segment, USHORT size);
void StC_LoginResponsePacketHandler(Session* session, char* segment, USHORT size);
void StC_AlreadyInUserPacketHandler(Session* session, char* segment, USHORT size);
void StC_UserDataPacketHandler(Session* session, char* segment, USHORT size);
void StC_UserFieldPacketHandler(Session* session, char* segment, USHORT size);
void StC_UserScorePacketHandler(Session* session, char* segment, USHORT size);
void StC_UserCurrentPiecePacketHandler(Session* session, char* segment, USHORT size);
void StC_ChallengerDataPacketHandler(Session* session, char* segment, USHORT size);
void StC_UserLosePacketHandler(Session* session, char* segment, USHORT size);
void StC_UserExitPacketHandler(Session* session, char* segment, USHORT size);
void StC_GameStartPacketHandler(Session* session, char* segment, USHORT size);
void DefaultPacketHandler(Session* session, char* segment, USHORT size);

class ClientPacketHandler {
	SINGLETON(ClientPacketHandler);

public:
	void Init();
	void HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size);

private:
	void Register();

private:
	map<PacketType, HandlerFunc> _func;
};

