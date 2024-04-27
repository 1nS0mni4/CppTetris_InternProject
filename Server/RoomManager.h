#pragma once
#include "pch.h"
#include "Defines.h"

#include "Session.h"
#include "Room.h"

class RoomManager {
	SINGLETON(RoomManager);

public:
	void EnterRoom(Session* session);
	void ExitRoom(Session* session);

public:
	void UpdateRooms();

private:
	queue<Room*> _rooms;
	map<UINT32, Room*> _onUpdates;

private:
	mutex m_room;
	atomic<bool> _isClosed;
};

