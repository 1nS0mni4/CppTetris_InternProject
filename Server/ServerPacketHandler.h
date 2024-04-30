#pragma once
#include "pch.h"
#include "ClientSession.h"
#include "PacketHandler.h"

void DefaultPacketHandler(Session* session, char* segment, USHORT size);
void TestPacketHandler(Session* session, char* segment, USHORT size);
void CtS_LoginRequestPacketHandler(Session* session, char* segment, USHORT size);
void CtS_MatchingRequestPacketHandler(Session* session, char* segment, USHORT size);
void CtS_NotifyFieldPacketHandler(Session* session, char* segment, USHORT size);
void CtS_NotifyScorePacketHandler(Session* session, char* segment, USHORT size);
void CtS_NotifyCurrentPiecePacketHandler(Session* session, char* segment, USHORT size);
void CtS_NotifyLosePacketHandler(Session* session, char* segment, USHORT size);

class ServerPacketHandler : public PacketHandler {
	SINGLETON(ServerPacketHandler);

public:
	void Init() override;
	void HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size) override;

private:
	void Register() override;
};

