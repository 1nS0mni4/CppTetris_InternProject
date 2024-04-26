#include "pch.h"
#include "SendBuffer.h"


SendBuffer::SendBuffer() {
	_segment = (char*)malloc(SENDBUF_SIZE);
	memset(_segment, 0, SENDBUF_SIZE);
	_cursor = _used = 0;
}

SendBuffer::~SendBuffer() {
	free(_segment);
}

void SendBuffer::Reorganize() {
	if (_cursor != _used) {
		strcpy_s(_segment, SENDBUF_SIZE, &(_segment[_used]));
		_cursor -= _used;
		_used = 0;
		return;
	}

	_cursor = _used = 0;
}

void SendBuffer::setUsed(int used) {
	lock_guard<std::mutex> guard(_mtx);
	if (_used + used > _cursor) {
		puts("[Error] SendBuffer::setUsed(int) : segment overflow");
		return;
	}

	_used += used;
}