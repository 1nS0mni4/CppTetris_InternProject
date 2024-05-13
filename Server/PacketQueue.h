#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"


#define DELETEPOOL(x)\
{\
	if (x.size() > 0) {\
		while (!x.empty()) {\
			PacketData* deleted = x.back(); x.pop_back();\
			delete deleted;\
		}\
	}\
}


struct PacketData {
	Session* session;
	char* segment;
	USHORT packetID;
	USHORT size;
};

class PacketQueue {
public:
	static PacketQueue& GetInstance() {
		static PacketQueue instance;
		return instance;
	}

	PacketQueue();
	~PacketQueue();
	PacketQueue(const PacketQueue& other) = delete;
	PacketQueue& operator=(const PacketQueue& other) = delete;
	PacketQueue(const PacketQueue&& other) = delete;
	PacketQueue& operator=(const PacketQueue&& other) = delete;


public:
	void Push(Session* session, char* segment, USHORT packetID, USHORT size);
	void Flush();
	PacketData* GetPD();
	void ReleasePD(vector<PacketData*>& releasee);
	void Close();

protected:
	char* WriteSegment(char* segment, int size);

private:
	vector<PacketData*> store, fetch, pool;
	atomic<bool> _isClosed;
	atomic<int> poolSize;
	char buffer[PACKETQUEUE_BUF_SIZE];
	atomic<int> _write, _read;
	std::mutex mtx;
};

