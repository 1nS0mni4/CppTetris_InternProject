#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"
#include "Packet.h"

class PacketHandler {
	SINGLETON(PacketHandler);

private:


public:
	virtual int HandlePacket(Session* session, Packet* packet, USHORT packetID);
};

