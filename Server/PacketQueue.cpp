#include "pch.h"
#include "PacketQueue.h"
#include "ServerPacketHandler.h"

PacketQueue::PacketQueue() {
	_isClosed = false;

	lock_guard<std::mutex> guard(mtx);

	for (int i = 0; i < PACKETDATA_POOL_SIZE; i++) {
		pool.push_back(new PacketData());
	}
	poolSize = pool.size();
	_write = _read = 0;
}

PacketQueue::~PacketQueue() {
	lock_guard<std::mutex> guard(mtx);
	DELETEPOOL(store);
	DELETEPOOL(fetch);
	DELETEPOOL(pool);
}

void PacketQueue::Push(Session* session, char* segment, USHORT packetID, USHORT size) {
	PacketData* data = GetPD();

	data->session = session;
	data->segment = WriteSegment(segment, size);
	data->packetID = packetID;
	data->size = size;

	lock_guard<std::mutex> guard(mtx);
	store.push_back(data);
}

char* PacketQueue::WriteSegment(char* segment, int size) {
	if (_write + size >= PACKETQUEUE_BUF_SIZE) {
		_write = 0;
	}

	char* start = &buffer[_write];
	_write += size;
	memcpy(start, segment, size);
	return start;
}

void PacketQueue::Flush() {
	while (_isClosed == false) {
		vector<PacketData*> flushed;
		{
			lock_guard<std::mutex> guard(mtx);

			int fetched = 0;
			while (fetch.empty() == false) {
				PacketData* data = fetch.back();
				fetch.pop_back();

				ServerPacketHandler::GetInstance().HandlePacket(data->session, data->segment, data->packetID, data->size);
				flushed.push_back(data);
				fetched++;
			}

			//cout << "Fetched: " << fetched << '\n';
		}
		ReleasePD(flushed);

		if (store.size() > 0) {
			lock_guard<std::mutex> guard(mtx);

			while (store.empty() == false) {
				fetch.push_back(store.back());
				store.pop_back();
			}

			store.clear();
		}
	}
}

PacketData* PacketQueue::GetPD() {
	PacketData* ret;
	while (pool.empty());

	lock_guard<std::mutex> guard(mtx);
	ret = pool.back();
	pool.pop_back();

	return ret;
}

void PacketQueue::ReleasePD(vector<PacketData*>& releasee) {
	lock_guard<std::mutex> guard(mtx);
	int size = releasee.size();

	for (int i = 0; i < size; i++) {
		PacketData* data = releasee[i];
		memset(data, 0, sizeof(PacketData));
		pool.push_back(data);
	}
	poolSize.fetch_add(size);
}

void PacketQueue::Close() { _isClosed = true; }