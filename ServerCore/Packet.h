#pragma once
#pragma pack(1)
#include "pch.h"

enum PacketType : USHORT{
	TestPacket = 0,
	CtS_LoginAccess = 1,

};

class Packet {
protected:
	USHORT _packetID;
	USHORT _size;

public:
	Packet(USHORT packetID);
	virtual ~Packet();

public:
	virtual int Read(char* segment);
	virtual int Write(char* buffer);

protected:
	template <typename T>
	T* getSegment(char* segment, int offset, int size);
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

private:
	int _data;

public:
	int getData() { return _data; }
};

class CtS_LoginAccessPacket : public Packet {
public:
	CtS_LoginAccessPacket();
	~CtS_LoginAccessPacket() override;

	int Read(char* segment) override;
	int Write(char* buffer) override;

private:
	std::string _id;
	std::string _pwd;

public:
	std::string getID() { return _id; }
	std::string getPWD() { return _pwd; }
};
