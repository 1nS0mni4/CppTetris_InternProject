#include "pch.h"
#include "RoomManager.h"

RoomManager::RoomManager() {
	lock_guard<mutex> guard(m_room);

	for (UINT32 i = 0; i < MAX_ROOM_COUNT; i++) {
		_rooms.push(new Room(i));
	}
}

RoomManager::~RoomManager() {
	lock_guard<mutex> guard(m_room);
	while(_rooms.empty() == false) {
		Room* room = _rooms.front();
		_rooms.pop();
		
		delete room;
	}
}

void RoomManager::EnterRoom(Session* session) {
	lock_guard<mutex> guard(m_room);
	if (_rooms.size() <= 0)
		return;

	Room* room = _rooms.front();

	room->Enter(session);

	if (room->IsFull()) {
		_rooms.pop();
		_onUpdates[room->GetID()] = room;

		room->Start();
	}
}

void RoomManager::ExitRoom(Session* session) {
	lock_guard<mutex> guard(m_room);
	if (_rooms.size() <= 0)
		return;

	Room* room = _rooms.front();

	room->Exit(session);

	if (room->IsEmpty() == 0) {
		_onUpdates[room->GetID()] = 0;
		_rooms.push(room);

		room->Clear();
	}
}

void RoomManager::UpdateRooms() {
	while (!_isClosed) {
		lock_guard<mutex> guard(m_room);
		if (_onUpdates.size() <= 0)
			break;

		for (auto roomIt = _onUpdates.begin(); roomIt != _onUpdates.end(); roomIt++) {
			Room* room = roomIt->second;
			if (room == 0)
				continue;

			room->Update();
		}
	}
}