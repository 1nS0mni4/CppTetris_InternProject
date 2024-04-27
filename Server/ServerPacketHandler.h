#pragma once
#include "pch.h"
#include "ClientSession.h"
#include "PacketHandler.h"

static void TestPacketHandler(Session* session, char* segment, USHORT size);

class ServerPacketHandler : public PacketHandler {
	SINGLETON(ServerPacketHandler);

public:
	void Init() override;
	void HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size) override;

private:
	void Register() override;
};

