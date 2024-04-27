#pragma once
#include "Defines.h"
#include "Session.h"

#define PACKETDATA_POOL_SIZE		1000

#define DELETEPOOL(x, m)\
{\
	lock_guard<mutex> guard(m);\
	if (x.size() > 0) {\
		while (!x.empty()) {\
			PacketData* deleted = x.back(); x.pop_back();\
			delete deleted;\
		}\
	}\
}

typedef struct PacketData {
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

private:
	vector<PacketData*> store, fetch, pool;
	atomic<bool> _isClosed;
	atomic<int> poolSize;
	std::mutex m_store, m_fetch, m_pool;
};

