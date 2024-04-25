#pragma once
#include "pch.h"
#include "ClientSession.h"
#include "Packet.h"

typedef void (* HandlerFunc)(Session* session, Packet* packet);

class ServerPacketHandler {
	SINGLETON(ServerPacketHandler);

public:
	void Init();
	void HandlePacket(Session* session, Packet packet, USHORT packetID);

private:
	void Register();


private:
	map<PacketType, HandlerFunc> _func;

};

