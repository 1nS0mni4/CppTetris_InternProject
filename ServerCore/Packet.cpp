#include "pch.h"
#include "Packet.h"


Packet::Packet(USHORT packetID) : _packetID(packetID), _size(0) { }
Packet::~Packet() { }

int Packet::Read(char* segment) {
	int offset = 0;
	_packetID = ((USHORT*)segment)[0 + offset];
	offset += sizeof(USHORT);
	_size =		((USHORT*)segment)[0 + offset];
	offset += sizeof(USHORT);

	return offset;
}

int Packet::Write(char* buffer) {
	int offset = 0;

	((USHORT*)buffer)[0 + offset] = _packetID;
	offset += sizeof(_packetID);
	((USHORT*)buffer)[0 + offset] = _size;
	offset += sizeof(_size);

	return offset;
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

	//TODO: 내용물 채우기.. 귀찮

	return offset;
}

TestPacket::TestPacket() : Packet(PacketType::TestPacket){ }
TestPacket::~TestPacket() { }

int TestPacket::Read(char* segment) {
	int offset = Packet::Read(segment);

	_data = *Packet::getSegment<int>(segment, offset, sizeof(int));

	return offset;
}

int TestPacket::Write(char* buffer) {
	int offset = Packet::Write(buffer);

	*Packet::getSegment<int>(buffer, offset, sizeof(int)) = _data;

	return offset;
}
