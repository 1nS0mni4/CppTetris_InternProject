#include "pch.h"
#include "Packet.h"
#include "Defines.h"


Packet::Packet() : _packetID(0), _size(0) {}
Packet::Packet(PacketType packetID) : _packetID((USHORT)packetID), _size(0) { }
Packet::~Packet() { }

int Packet::Read(char* segment) {
	int offset = 0;
	_packetID = ((USHORT*)segment)[0];
	offset += sizeof(USHORT);
	_size =		((USHORT*)segment)[1];
	offset += sizeof(USHORT);

	return offset;
}

int Packet::Write(char* buffer) {
	((USHORT*)buffer)[0] = _packetID;
	((USHORT*)buffer)[1] = _size;
	return _size;
}

CtS_LoginAccessPacket::CtS_LoginAccessPacket() : Packet(PacketType::CtS_LoginAccess) { }
CtS_LoginAccessPacket::~CtS_LoginAccessPacket() {}

int CtS_LoginAccessPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	//TODO: 문자열은 앞에 USHORT로 먼저 사이즈, 그 다음 데이터가 온다.

	return index;
}

int CtS_LoginAccessPacket::Write(char* buffer) {
	int index = Packet::Write(buffer);

	//TODO: 내용 채우기.. 귀찮

	return index;
}

TestPacket::TestPacket() : Packet(PacketType::TestPacket) { data = 0; }
TestPacket::~TestPacket() { }

int TestPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	data = *((int*)(&(segment[index])));
	index += sizeof(int);

	return index;
}

int TestPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;
	*((int*)&(buffer[_size])) = data;
	_size += sizeof(int);

	Packet::Write(buffer);
	return _size;
}

#define PROMOTE(x) new (packet) x;
char* Packet::Promote(Packet* packet, char* segment) {
	USHORT packetID = ((USHORT*)segment)[1];

	switch (packetID) {
		case (USHORT)PacketType::TestPacket:		PROMOTE(TestPacket)				break;
		case (USHORT)PacketType::CtS_LoginAccess:	PROMOTE(CtS_LoginAccessPacket)	break;

		default:break;
	}

	return (char*)packet;
}
