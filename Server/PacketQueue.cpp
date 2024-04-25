#include "pch.h"
#include "PacketQueue.h"
#include "ServerPacketHandler.h"

PacketQueue::PacketQueue() { }
PacketQueue::~PacketQueue() { }

void PacketQueue::Push(Session* session, Packet& packet, USHORT packetID) {
	lock_guard<std::mutex> guard(m_Store);
	store.push_back({ session, packet, packetID });
}

void PacketQueue::Flush() {
	while (true) {
		lock_guard<std::mutex> guard(m_Fetch);

		while (fetch.empty() == false) {
			PacketData data = fetch.back();
			fetch.pop_back();

			ServerPacketHandler::GetInstance().HandlePacket(data.session, data.packet, data.packetID);
		}

		if (store.size() > 0) {
			lock_guard<std::mutex> guard(m_Store);
			fetch = store;
			store.clear();
		}
	}
	
}