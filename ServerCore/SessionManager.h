#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"


class SessionManager {
	SINGLETON(SessionManager);

private:
	atomic<int> _sessionCnt;
	std::mutex _mutex;

public:
	Session* CreateSession();
	void RemoveSession(int sessionID);
	map<int, shared_ptr<Session>> _sessions;
	priority_queue<int, vector<int>, less<int>> _onClosed;
};

