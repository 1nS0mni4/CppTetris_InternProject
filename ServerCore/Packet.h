#pragma once
#include "pch.h"

using Segment = char*;

enum class PacketType : USHORT {
	TestPacket = 0,
	CtS_LoginRequest = 1,
	StC_LoginResponse,
	StC_UserData,
	CtS_MatchingRequest,

	CtS_NotifyField,
	CtS_NotifyScore,

	StC_UserField,
	StC_UserScore,

};

class Packet {
protected:
	USHORT _packetID;
	USHORT _size;

public:
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

class TestPacket : public Packet {
public:
	TestPacket();
	~TestPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int data;
};

class CtS_LoginRequestPacket : public Packet {
public:
	CtS_LoginRequestPacket();
	~CtS_LoginRequestPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	std::string id;
};

class StC_LoginResponsePacket : public Packet {
public:
	StC_LoginResponsePacket();
	~StC_LoginResponsePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int result;
};

class StC_UserDataPacket : public Packet {
public:
	StC_UserDataPacket();
	~StC_UserDataPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int win;
	int lose;
	int maxScore;
};

class CtS_MatchingRequestPacket : public Packet {
public:
	CtS_MatchingRequestPacket();
	~CtS_MatchingRequestPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;
};

class CtS_NotifyFieldPacket : public Packet{
public:
	CtS_NotifyFieldPacket();
	~CtS_NotifyFieldPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	char* field;
};

class CtS_NotifyScorePacket : public Packet{
public:
	CtS_NotifyScorePacket();
	~CtS_NotifyScorePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int score;
};

class StC_UserFieldPacket : public Packet{
public:
	StC_UserFieldPacket();
	~StC_UserFieldPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	char* field;
};

class StC_UserScorePacket : public Packet {
public:
	StC_UserScorePacket();
	~StC_UserScorePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int score;
};

