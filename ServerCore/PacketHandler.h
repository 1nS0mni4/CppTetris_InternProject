#pragma once
#include "pch.h"
#include "Session.h"
#include "Packet.h"

typedef void (*HandlerFunc)(Session* session, char* segment, USHORT size);

class PacketHandler {
public:
	virtual void Init() = 0;
	virtual void HandlePacket(Session* session, char* packet, USHORT packetID, USHORT size) = 0;

protected:
	virtual void Register() = 0;

protected:
	map<PacketType, HandlerFunc> _func;
};

