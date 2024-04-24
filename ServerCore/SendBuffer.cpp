#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer() {
	_segment = (char*)malloc(SENDBUF_SIZE);
	memset(_segment, 0, SENDBUF_SIZE);

}

SendBuffer::~SendBuffer() {
	free(_segment);
}

char* SendBuffer::getFreeSegment(int size) {

	char* registSegment = nullptr;


	return registSegment;
}

void SendBuffer::Reorganize() {

}

char* SendBuffer::getUsedSegment(int size) {


	char* usedSegment = nullptr;

	_isStandby.unlock();
	return usedSegment;
}