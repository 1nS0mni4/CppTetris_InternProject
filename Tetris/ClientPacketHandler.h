#pragma once
#include "CorePch.h"
#include "Packet.h"
#include "ServerSession.h"

typedef void (*HandlerFunc)(Session* session, char* segment, USHORT size);

#define FUNC_DCLR(x) void x##Handler(Session* session, char* segment, USHORT size)
FUNC_DCLR(TestPacket);
FUNC_DCLR(StC_MatchingResultPacket);
FUNC_DCLR(StC_LoginResponsePacket);
FUNC_DCLR(StC_AlreadyInUserPacket);
FUNC_DCLR(StC_UserDataPacket);
FUNC_DCLR(StC_UserFieldPacket);
FUNC_DCLR(StC_UserScorePacket);
FUNC_DCLR(StC_UserCurrentPiecePacket);
FUNC_DCLR(StC_ChallengerDataPacket);
FUNC_DCLR(StC_UserLosePacket);
FUNC_DCLR(StC_UserExitPacket);
FUNC_DCLR(StC_GameStartPacket);


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

