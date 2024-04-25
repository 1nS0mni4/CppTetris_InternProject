#include "pch.h"
#include "SendBuffer.h"


SendBuffer::SendBuffer() {
	_segment = (char*)malloc(SENDBUF_SIZE);
	memset(_segment, 0, SENDBUF_SIZE);
}

SendBuffer::~SendBuffer() {
	free(_segment);
}

void SendBuffer::Reorganize() {
	memset(_segment, 0, _cursor);
	_cursor = _used = 0;
}

template <typename T>
char* SendBuffer::write(T& packet, OUT int& size) {
	lock_guard<std::mutex> guard(_mtx);
	int required = _cursor + sizeof(T);
	if (required >= SENDBUF_SIZE)
		Reorganize();

	char* startPos = &(_segment[_cursor]);
	size = packet.Write(startPos);
	_cursor += size;

	return (char*)(((T*)startPos)[0]);
}

void SendBuffer::setUsed(int used) {
	lock_guard<std::mutex> guard(_mtx);
	if (_used + used > _cursor)
		return;

	_used += used;
}