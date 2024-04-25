#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"

typedef struct PacketData {
	Session* session;
	Packet packet;
	USHORT packetID;
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
	void Push(Session* session, Packet& packet, USHORT packetID);
	
public:
	void Flush();

private:
	vector<PacketData> store, fetch;
	std::mutex m_Store, m_Fetch;
};

