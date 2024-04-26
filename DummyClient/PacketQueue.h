#pragma once
#include "pch.h"
#include "Defines.h"
#include "ServerSession.h"

#define PACKETDATA_POOL_SIZE		1000

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
	vector<PacketData*> GetPD(int count = 1);
	void ReleasePD(vector<PacketData*>& releasee);
	void Close();

private:
	vector<PacketData*> store, fetch, pool;
	atomic<bool> _isClosed;
	std::mutex m_store, m_fetch, m_pool;
};

