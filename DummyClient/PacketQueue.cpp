#include "pch.h"
#include "PacketQueue.h"
#include "ClientPacketHandler.h"

PacketQueue::PacketQueue() {
	_isClosed = false;

	lock_guard<std::mutex> guard(m_pool);

	for (int i = 0; i < PACKETDATA_POOL_SIZE; i++) {
		pool.push_back(new PacketData());
	}
}

PacketQueue::~PacketQueue() {
	if (store.size() > 0) {
		while (!store.empty()) {
			PacketData* deleted = store.back(); store.pop_back();
			delete deleted;
		}
	}
}

void PacketQueue::Push(Session* session, char* segment, USHORT packetID, USHORT size) {
	PacketData* data = GetPD().back();
	data->session = session;
	data->segment = segment;
	data->packetID = packetID;
	data->size = size;

	lock_guard<std::mutex> guard(m_store);
	store.push_back(data);
}

void PacketQueue::Flush() {
	while (_isClosed == false) {
		lock_guard<std::mutex> guard(m_fetch);
		vector<PacketData*> flushed;

		while (fetch.empty() == false) {
			PacketData* data = fetch.back();
			fetch.pop_back();

			ClientPacketHandler::GetInstance().HandlePacket(data->session, data->segment, data->packetID, data->size);
			flushed.push_back(data);
		}

		if (store.size() > 0) {
			lock_guard<std::mutex> guard(m_store);
			fetch = store;
			store.clear();
		}

		ReleasePD(flushed);
	}
}

vector<PacketData*> PacketQueue::GetPD(int count) {
	vector<PacketData*> ret;

	lock_guard<std::mutex> guard(m_pool);
	for (int i = 0; i < count; i++) {
		ret.push_back(pool.back());
		pool.pop_back();
	}

	return ret;
}

void PacketQueue::ReleasePD(vector<PacketData*>& releasee) {
	lock_guard<std::mutex> guard(m_pool);
	int size = releasee.size();

	for (int i = 0; i < size; i++) {
		PacketData* data = releasee[i];
		memset(data, 0, sizeof(PacketData));
		pool.push_back(data);
	}
}

void PacketQueue::Close() { _isClosed = true; }