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
	_size += sizeof(_packetID);
	_size += sizeof(_size);

	((USHORT*)buffer)[1] = _size;
	return _size;
}

template <typename T>
T* Packet::getSegment(char* segment, int offset, int size) {
	char* startPos = &(segment[offset]);
	return &(((T*)startPos)[0]);
}

CtS_LoginAccessPacket::CtS_LoginAccessPacket() : Packet(PacketType::CtS_LoginAccess) { }
CtS_LoginAccessPacket::~CtS_LoginAccessPacket() {}

int CtS_LoginAccessPacket::Read(char* segment) {
	int offset = Packet::Read(segment);

	//TODO: 문자열은 앞에 USHORT로 먼저 사이즈, 그 다음 데이터가 온다.

	return offset;
}

int CtS_LoginAccessPacket::Write(char* buffer) {
	int offset = Packet::Write(buffer);

	//TODO: 내용 채우기.. 귀찮

	return offset;
}

TestPacket::TestPacket() : Packet(PacketType::TestPacket){ }
TestPacket::~TestPacket() { }

int TestPacket::Read(char* segment) {
	int offset = Packet::Read(segment);

	data = *Packet::getSegment<int>(segment, offset, sizeof(int));
	offset += sizeof(int);

	return offset;
}

int TestPacket::Write(char* buffer) {
	*Packet::getSegment<int>(buffer, PACKET_HEADER_SIZE + _size, sizeof(int)) = data;
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
