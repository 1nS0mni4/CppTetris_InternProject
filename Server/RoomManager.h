#pragma once
#include "pch.h"
#include "Defines.h"

#include "ClientSession.h"
#include "Room.h"

class RoomManager {
	SINGLETON(RoomManager);

public:
	void EnterRoom(ClientSession* session);
	void ExitRoom(ClientSession* session);

public:
	void UpdateRooms();
	void Close();

private:
	queue<Room*> _rooms;
	map<UINT32, Room*> _onUpdates;

private:
	mutex m_room;
	atomic<bool> _isClosed;
};

