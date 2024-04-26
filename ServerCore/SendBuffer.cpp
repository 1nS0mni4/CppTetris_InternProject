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

void SendBuffer::setUsed(int used) {
	lock_guard<std::mutex> guard(_mtx);
	if (_used + used > _cursor)
		return;

	_used += used;
}