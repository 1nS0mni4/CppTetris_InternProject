#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"

class Room {
public:
	Room(UINT32 roomID);
	~Room();

public:
	UINT32 GetID() { return _roomID; }
	bool IsFull() { return _sessions.size() == MAX_ROOM_CAPACITY; }
	bool IsEmpty() { return _sessions.size() <= 0; }

public:
	void Enter(Session* session);
	void Exit(Session* session);

	void Update();
	
	void Start();
	void Clear();

	Session* GetOtherSession(UINT32 sessionID);
	void UpdateScore(UINT32 sessionID, int score);
	void SaveScore();

private:
	UINT32 _roomID;
	map<UINT32, Session*> _sessions;
	map<UINT32, int> _scores;

private:

};

