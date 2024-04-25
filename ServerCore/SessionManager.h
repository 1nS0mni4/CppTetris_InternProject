#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"

template <typename T>
class SessionManager {
public:
	static SessionManager<T>& GetInstance() {
		static SessionManager<T> instance;
		return instance;
	}

private:
	SessionManager() {

	}
	~SessionManager(){
		_sessions.clear();
	}

private:
	atomic<int> _sessionCnt;
	std::mutex _mutex;

public:
	void Init() {
		AddSession<T>(INIT_SESSION_COUNT);

		_sessionCnt = 0;
	}

	Session* CreateSession() {
		int sessionID = -1;
		{
			lock_guard<std::mutex> guard(_mutex);

			if (_onClosed.size() <= 0) {
				AddSession(5);
			}

			sessionID = _onClosed.top();
			_onClosed.pop();
		}

		_sessionCnt++;
		return _sessions[sessionID];
	}
	void RemoveSession(int sessionID) {
		lock_guard<std::mutex> guard(_mutex);
		_onClosed.push(sessionID);
		_sessionCnt--;
	}

private:
	void AddSession(int count) {
		int idx = _sessions.size();
		for (int i = idx; i <= idx + count; i++) {
			_sessions.push_back(new T(i));
			_onClosed.push(i);
		}
	}

private:
	vector<T*> _sessions;
	priority_queue<int, vector<int>, less<int>> _onClosed;

};

