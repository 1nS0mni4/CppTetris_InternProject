#pragma once
#include "pch.h"
#include "Defines.h"

using Segment = char*;

enum class PacketType : USHORT {
	Test = 0,
	StC_AlreadyInUser,
	CtS_LoginRequest,
	StC_GameStart,
	CtS_MatchingRequest,
	StC_UserLose,
	CtS_NotifyField,
	StC_UserData,
	StC_UserExit,
	CtS_NotifyScore,
	StC_LoginResponse,
	StC_UserCurrentPiece,
	CtS_NotifyCurrentPiece,
	StC_UserScore,
	CtS_NotifyLose,
	StC_ChallengerData,
	StC_MatchingResult,
	StC_UserField,

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
	volatile int data;
};



class CtS_LoginRequestPacket : public Packet {
public:
	CtS_LoginRequestPacket();
	~CtS_LoginRequestPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	wchar_t name[NAME_LEN];
	USHORT nameLen;
};

class CtS_MatchingRequestPacket : public Packet {
public:
	CtS_MatchingRequestPacket();
	~CtS_MatchingRequestPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;
};

class CtS_NotifyFieldPacket : public Packet {
public:
	CtS_NotifyFieldPacket();
	~CtS_NotifyFieldPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	unsigned char field[FIELD_LEN];
};

class CtS_NotifyScorePacket : public Packet {
public:
	CtS_NotifyScorePacket();
	~CtS_NotifyScorePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int score;
};

class CtS_NotifyCurrentPiecePacket : public Packet {
public:
	CtS_NotifyCurrentPiecePacket();
	~CtS_NotifyCurrentPiecePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int currentPiece;
	int rotation;
	int currentX;
	int currentY;
};

class CtS_NotifyLosePacket : public Packet {
public:
	CtS_NotifyLosePacket();
	~CtS_NotifyLosePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;
};


//
class StC_MatchingResultPacket : public Packet {
public:
	StC_MatchingResultPacket();
	~StC_MatchingResultPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;
};

//
class StC_LoginResponsePacket : public Packet {
public:
	StC_LoginResponsePacket();
	~StC_LoginResponsePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int result;
};

//
class StC_AlreadyInUserPacket : public Packet{
public:
	StC_AlreadyInUserPacket();
	~StC_AlreadyInUserPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	wchar_t name[NAME_LEN];
	USHORT nameLen;
};

//
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

//
class StC_UserFieldPacket : public Packet {
public:
	StC_UserFieldPacket();
	~StC_UserFieldPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	unsigned char field[FIELD_LEN];
};

//
class StC_UserScorePacket : public Packet {
public:
	StC_UserScorePacket();
	~StC_UserScorePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int score;
};

//
class StC_UserCurrentPiecePacket : public Packet {
public:
	StC_UserCurrentPiecePacket();
	~StC_UserCurrentPiecePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	int currentPiece;
	int rotation;
	int currentX;
	int currentY;
};

//
class StC_ChallengerDataPacket : public Packet {
public:
	StC_ChallengerDataPacket();
	~StC_ChallengerDataPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

public:
	wchar_t name[NAME_LEN];
	USHORT nameLen;
};

//
class StC_UserLosePacket : public Packet {
public:
	StC_UserLosePacket();
	~StC_UserLosePacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;
};

//
class StC_UserExitPacket : public Packet {
public:
	StC_UserExitPacket();
	~StC_UserExitPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;
};


class StC_GameStartPacket : public Packet {
public:
	StC_GameStartPacket();
	~StC_GameStartPacket();

	int Read(char* segment);
	int Write(char* buffer);
};