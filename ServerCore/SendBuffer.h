#pragma once
#include "pch.h"
#include "Defines.h"
#include "Packet.h"
#include <span>
#include <spanstream>

class SendBuffer {
	SINGLETON(SendBuffer);

public:
	template <typename T = Packet>
	char* write(T& packet, OUT int& size) {
		lock_guard<std::mutex> guard(_mtx);
		int required = _cursor + sizeof(T);
		if (required >= SENDBUF_SIZE)
			Reorganize();

		char* startPos = &(_segment[_cursor]);
		size = packet.Write(startPos);
		_cursor += size;

		return startPos;
	}
	void setUsed(int used);

private:
	void Reorganize();

private:
	char* _segment;
	int _cursor;
	int _used;
	std::mutex _mtx;
};

