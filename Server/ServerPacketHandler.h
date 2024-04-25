#pragma once
#include "pch.h"
#include "PacketHandler.h"

typedef
void (* HandlerFunc)(Session* session, Packet* packet);

class ServerPacketHandler : public PacketHandler {
protected:
	ServerPacketHandler();
	~ServerPacketHandler() override;

public:
	virtual int HandlePacket(Session* session, Packet* packet, USHORT packetID) override;

private:
	void Register();


private:
	map<PacketType, HandlerFunc> _func;

};

