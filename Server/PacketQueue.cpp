#include "pch.h"
#include "PacketQueue.h"
#include "ServerPacketHandler.h"

PacketQueue::PacketQueue() { 
	_isClosed = false;

	lock_guard<std::mutex> guard(m_pool);

	for (int i = 0; i < PACKETDATA_POOL_SIZE; i++) {
		pool.push_back(new PacketData());
	}
	poolSize = pool.size();
}

PacketQueue::~PacketQueue() { 
	DELETEPOOL(store, m_store);
	DELETEPOOL(fetch, m_fetch);
	DELETEPOOL(pool, m_pool);
}

void PacketQueue::Push(Session* session, char* segment, USHORT packetID, USHORT size) {
	PacketData* data = GetPD();
	
	data->session = session;
	data->segment = segment;
	data->packetID = packetID;
	data->size = size;
	
	lock_guard<std::mutex> guard(m_store);
	store.push_back(data);
}

void PacketQueue::Flush() {
	while (_isClosed == false) {
		SleepEx(10, TRUE);
		{
			lock_guard<std::mutex> guard(m_fetch);
			vector<PacketData*> flushed;
			int fetched = 0;
			while (fetch.empty() == false) {
				PacketData* data = fetch.back();
				fetch.pop_back();

				ServerPacketHandler::GetInstance().HandlePacket(data->session, data->segment, data->packetID, data->size);
				flushed.push_back(data);
				fetched++;
			}
			ReleasePD(flushed);
			cout << "Fetched: " << fetched << '\n';
		}

		if (store.size() > 0) {
			lock_guard<std::mutex> guard(m_store);
			fetch = store;
			store.clear();
		}
	}
}

PacketData* PacketQueue::GetPD() {
	PacketData* ret;
	while (pool.empty());

	lock_guard<std::mutex> guard(m_pool);
	ret = pool.back();
	pool.pop_back();
	
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
	poolSize.fetch_add(size);
}

void PacketQueue::Close() { _isClosed = true; }