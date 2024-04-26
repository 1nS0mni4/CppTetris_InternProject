#pragma once
#include "pch.h"
#include "ClientSession.h"
#include "Packet.h"

typedef void (* HandlerFunc)(Session* session, char* segment, USHORT size);

static void TestPacketHandler(Session* session, char* segment, USHORT size);

class ServerPacketHandler {
	SINGLETON(ServerPacketHandler);

public:
	void Init();
	void HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size);

private:
	void Register();


private:
	map<PacketType, HandlerFunc> _func;

};

