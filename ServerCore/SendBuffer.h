#pragma once
#include "pch.h"
#include "Defines.h"

class SendBuffer {
	SINGLETON(SendBuffer);
public:
	char* getFreeSegment(int size);
	char* getUsedSegment(int size);

private:
	void Reorganize();

private:
	char* _segment;
	int _write;
	int _capacity;
	std::mutex _isStandby;
	
};

