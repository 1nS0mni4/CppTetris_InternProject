#include "pch.h"
#include "Packet.h"
#include "Defines.h"

#define WRITESEGMENT(x) *((x*)& buffer[_size])

#pragma region Packet
Packet::Packet(PacketType packetID) : _packetID((USHORT)packetID), _size(0) {}
Packet::~Packet() {}

int Packet::Read(char* segment) {
	int offset = 0;
	_packetID = ((USHORT*)segment)[0];
	offset += sizeof(USHORT);
	_size = ((USHORT*)segment)[1];
	offset += sizeof(USHORT);

	return offset;
}

int Packet::Write(char* buffer) {
	((USHORT*)buffer)[0] = _packetID;
	((USHORT*)buffer)[1] = _size;
	return _size;
}
#pragma endregion

#pragma region TestPacket
TestPacket::TestPacket() : Packet(PacketType::Test) { data = 0; }
TestPacket::~TestPacket() {}

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
#pragma endregion


#pragma region Client To Server

#pragma region CtS_LoginRequest
CtS_LoginRequestPacket::CtS_LoginRequestPacket() : Packet(PacketType::CtS_LoginRequest) {}
CtS_LoginRequestPacket::~CtS_LoginRequestPacket() {}

int CtS_LoginRequestPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	int nameLen = *((USHORT*)(&(segment[index])));
	index += sizeof(USHORT);
	memcpy(&(segment[index]), name, NAME_LEN);
	index += nameLen;

	return index;
}

int CtS_LoginRequestPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	WRITESEGMENT(USHORT) = NAME_LEN;
	_size += sizeof(USHORT);
	memcpy(&(buffer[_size]), name, NAME_LEN);
	_size += NAME_LEN;

	Packet::Write(buffer);
	return _size;
}
#pragma endregion

#pragma region CtS_MatchingRequest

CtS_MatchingRequestPacket::CtS_MatchingRequestPacket() : Packet(PacketType::CtS_MatchingRequest) { }
CtS_MatchingRequestPacket::~CtS_MatchingRequestPacket() {}

int CtS_MatchingRequestPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	return index;
}

int CtS_MatchingRequestPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region CtS_NotifyFieldPacket
CtS_NotifyFieldPacket::CtS_NotifyFieldPacket() : Packet(PacketType::CtS_NotifyField) {

}
CtS_NotifyFieldPacket::~CtS_NotifyFieldPacket() {}

int CtS_NotifyFieldPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	int len = *((USHORT*)(&(segment[index])));
	index += sizeof(USHORT);
	memcpy(field, &(segment[index]), len);
	index += len;

	return index;
}

int CtS_NotifyFieldPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	WRITESEGMENT(USHORT) = 216;
	_size += sizeof(USHORT);
	memcpy(&(buffer[_size]), field, 216);
	_size += 216;

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region CtS_NotifyScorePacket
CtS_NotifyScorePacket::CtS_NotifyScorePacket() : Packet(PacketType::CtS_NotifyScore) {}
CtS_NotifyScorePacket::~CtS_NotifyScorePacket() {}

int CtS_NotifyScorePacket::Read(char* segment) {
	int index = Packet::Read(segment);

	score = *((int*)(&(segment[index])));
	index += sizeof(int);

	return index;
}

int CtS_NotifyScorePacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;
	*((int*)&(buffer[_size])) = score;
	_size += sizeof(int);

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region CtS_NotifyCurrentPiecePacket
CtS_NotifyCurrentPiecePacket::CtS_NotifyCurrentPiecePacket() : Packet(PacketType::CtS_NotifyCurrentPiece) {}
CtS_NotifyCurrentPiecePacket::~CtS_NotifyCurrentPiecePacket() {}

int CtS_NotifyCurrentPiecePacket::Read(char* segment) {
	int index = Packet::Read(segment);

	currentPiece = *((int*)(&(segment[index])));
	index += sizeof(int);
	rotation = *((int*)(&(segment[index])));
	index += sizeof(int);
	currentX = *((int*)(&(segment[index])));
	index += sizeof(int);
	currentY = *((int*)(&(segment[index])));
	index += sizeof(int);

	return index;
}

int CtS_NotifyCurrentPiecePacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;
	*((int*)&(buffer[_size])) = currentPiece;
	_size += sizeof(int);
	*((int*)&(buffer[_size])) = rotation;
	_size += sizeof(int);
	*((int*)&(buffer[_size])) = currentX;
	_size += sizeof(int);
	*((int*)&(buffer[_size])) = currentY;
	_size += sizeof(int);

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region CtS_NotifyLosePacket
CtS_NotifyLosePacket::CtS_NotifyLosePacket() : Packet(PacketType::CtS_NotifyLose) {}
CtS_NotifyLosePacket::~CtS_NotifyLosePacket() {}

int CtS_NotifyLosePacket::Read(char* segment) {
	int index = Packet::Read(segment);

	return index;
}

int CtS_NotifyLosePacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	Packet::Write(buffer);
	return _size;
}

#pragma endregion


#pragma endregion


#pragma region Server To Client

#pragma region StC_MatchingResultPacket
StC_MatchingResultPacket::StC_MatchingResultPacket() : Packet(PacketType::StC_MatchingResult) {}

StC_MatchingResultPacket::~StC_MatchingResultPacket() {}

int StC_MatchingResultPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	return index;
}

int StC_MatchingResultPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	Packet::Write(buffer);
	return _size;
}
#pragma endregion

#pragma region StC_LoginResponse
StC_LoginResponsePacket::StC_LoginResponsePacket() : Packet(PacketType::StC_LoginResponse) {}
StC_LoginResponsePacket::~StC_LoginResponsePacket() {}

int StC_LoginResponsePacket::Read(char* segment) {
	int index = Packet::Read(segment);

	result = *((int*)(&(segment[index])));
	index += sizeof(int);

	return index;
}

int StC_LoginResponsePacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;
	*((int*)&(buffer[_size])) = result;
	_size += sizeof(int);

	Packet::Write(buffer);
	return _size;
}
#pragma endregion

#pragma region StC_AlreadyInUserPacket
StC_AlreadyInUserPacket::StC_AlreadyInUserPacket() : Packet(PacketType::StC_AlreadyInUser) {}
StC_AlreadyInUserPacket::~StC_AlreadyInUserPacket() {}

int StC_AlreadyInUserPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	int size = *((USHORT*)&(segment[index]));
	index += sizeof(USHORT);
	memcpy(name, segment, size);
	index += size;

	return index;
}

int StC_AlreadyInUserPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	*(USHORT*)(&(buffer[_size])) = NAME_LEN;
	_size += sizeof(USHORT);
	memcpy(&(buffer[_size]), name, NAME_LEN);
	_size += NAME_LEN;

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region StC_UserData

StC_UserDataPacket::StC_UserDataPacket() : Packet(PacketType::StC_UserData) {}
StC_UserDataPacket::~StC_UserDataPacket() {}

int StC_UserDataPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	win = *((int*)(&(segment[index])));
	index += sizeof(int);

	lose = *((int*)(&(segment[index])));
	index += sizeof(int);

	maxScore = *((int*)(&(segment[index])));
	index += sizeof(int);

	return index;
}

int StC_UserDataPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	*((int*)&(buffer[_size])) = win;
	_size += sizeof(int);

	*((int*)&(buffer[_size])) = lose;
	_size += sizeof(int);

	*((int*)&(buffer[_size])) = maxScore;
	_size += sizeof(int);

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region StC_UserFieldPacket

StC_UserFieldPacket::StC_UserFieldPacket() : Packet(PacketType::StC_UserField) { }
StC_UserFieldPacket::~StC_UserFieldPacket() {}

int StC_UserFieldPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	int len = *((USHORT*)(&(segment[index])));
	index += sizeof(USHORT);
	memcpy(field, &(segment[index]), len);
	index += len;

	return index;
}

int StC_UserFieldPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	WRITESEGMENT(USHORT) = 216;
	_size += sizeof(USHORT);
	//strcpy_s(&(buffer[_size]), SENDBUF_SIZE - _size, (char*)field);
	memcpy(&(buffer[_size]), field, 216);
	_size += 216;

	Packet::Write(buffer);
	return _size;
}



#pragma endregion

#pragma region StC_UserScorePacket
StC_UserScorePacket::StC_UserScorePacket() : Packet(PacketType::StC_UserScore) {}
StC_UserScorePacket::~StC_UserScorePacket() {}

int StC_UserScorePacket::Read(char* segment) {
	int index = Packet::Read(segment);

	score = *((int*)(&(segment[index])));
	index += sizeof(int);

	return index;
}

int StC_UserScorePacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;
	*((int*)&(buffer[_size])) = score;
	_size += sizeof(int);

	Packet::Write(buffer);
	return _size;
}
#pragma endregion

#pragma region Stc_UserCurrentPiecePacket
StC_UserCurrentPiecePacket::StC_UserCurrentPiecePacket() : Packet(PacketType::StC_UserCurrentPiece) {}
StC_UserCurrentPiecePacket::~StC_UserCurrentPiecePacket() {}

int StC_UserCurrentPiecePacket::Read(char* segment) {
	int index = Packet::Read(segment);

	currentPiece = *((int*)(&(segment[index])));
	index += sizeof(int);
	rotation = *((int*)(&(segment[index])));
	index += sizeof(int);
	currentX = *((int*)(&(segment[index])));
	index += sizeof(int);
	currentY = *((int*)(&(segment[index])));
	index += sizeof(int);

	return index;
}

int StC_UserCurrentPiecePacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;
	*((int*)&(buffer[_size])) = currentPiece;
	_size += sizeof(int);
	*((int*)&(buffer[_size])) = rotation;
	_size += sizeof(int);
	*((int*)&(buffer[_size])) = currentX;
	_size += sizeof(int);
	*((int*)&(buffer[_size])) = currentY;
	_size += sizeof(int);

	Packet::Write(buffer);
	return _size;
}
#pragma endregion

#pragma region StC_ChallengerDataPacket
Stc_ChallengerDataPacket::Stc_ChallengerDataPacket() : Packet(PacketType::StC_ChallengerData) {}

Stc_ChallengerDataPacket::~Stc_ChallengerDataPacket() {}

int Stc_ChallengerDataPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	int len = *((USHORT*)(&(segment[index])));
	index += sizeof(USHORT);
	memcpy(name, &(segment[index]), len);
	index += len;

	return 0;
}

int Stc_ChallengerDataPacket::Write(char* buffer) {
	WRITESEGMENT(USHORT) = 10;
	_size += sizeof(USHORT);
	memcpy(&(buffer[_size]), name, NAME_LEN);
	_size += 216;

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region StC_UserLosePacket
StC_UserLosePacket::StC_UserLosePacket() : Packet(PacketType::StC_UserLose) {}
StC_UserLosePacket::~StC_UserLosePacket() {}

int StC_UserLosePacket::Read(char* segment) {
	int index = Packet::Read(segment);

	return index;
}

int StC_UserLosePacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	Packet::Write(buffer);
	return _size;
}

#pragma endregion

#pragma region StC_UserExitPacket
StC_UserExitPacket::StC_UserExitPacket() : Packet(PacketType::StC_UserExit) {}
StC_UserExitPacket::~StC_UserExitPacket() {}

int StC_UserExitPacket::Read(char* segment) {
	int index = Packet::Read(segment);

	return index;
}

int StC_UserExitPacket::Write(char* buffer) {
	_size += PACKET_HEADER_SIZE;

	Packet::Write(buffer);
	return _size;
}

#pragma endregion


#pragma endregion


