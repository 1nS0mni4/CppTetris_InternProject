#pragma once
#include "pch.h"
#include "Defines.h"
#include "Packet.h"

class SendBuffer {
	SINGLETON(SendBuffer);

public:
	template <typename T = Packet>
	char* write(T& packet, OUT int& size);
	void setUsed(int used);

private:
	void Reorganize();

private:
	char* _segment;
	int _cursor;
	int _used;
	std::mutex _mtx;
};

