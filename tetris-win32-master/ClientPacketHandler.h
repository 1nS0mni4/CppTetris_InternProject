#pragma once
#include "Session.h"

typedef void (*HandlerFunc)(Session* session, char* segment, USHORT size);

static void TestPacketHandler(Session* session, char* segment, USHORT size);

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

