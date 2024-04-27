#pragma once
//#pragma pack(1)
#include "pch.h"

using Segment = char*;

enum class PacketType : USHORT {
	TestPacket = 0,
	CtS_LoginAccess = 1,

};

class Packet {
protected:
	USHORT _packetID;
	USHORT _size;

public:
	Packet();
	Packet(PacketType packetID);
	virtual ~Packet();

public:
	virtual int Read(char* segment);
	virtual int Write(char* buffer);

	USHORT packetID() { return _packetID; }
	USHORT size() { return _size; }

public:
	static char* Promote(Packet* packet, char* segment);
};

class GamePacket : public Packet{
public:
	GamePacket();
	~GamePacket() override;

	virtual int Read(char* segment);
	virtual int Write(char* buffer);
};

class TestPacket : public Packet {
public:
	TestPacket();
	~TestPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int data;
};

class CtS_LoginAccessPacket : public Packet {
public:
	CtS_LoginAccessPacket();
	~CtS_LoginAccessPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

private:
	std::string id;
	std::string pwd;
};