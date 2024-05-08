#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"

template <typename T = Session>
class SessionManager {
public:
	static SessionManager<T>& GetInstance() {
		static SessionManager<T> instance;
		return instance;
	}

protected:
	SessionManager() {
		_sessions.push_back(nullptr);
	}
	virtual ~SessionManager(){
		_sessions.clear();
	}

protected:
	atomic<int> _sessionCnt;
	std::mutex _mutex;

public:
	void Init() {
		AddSession(INIT_SESSION_COUNT);

		_sessionCnt = 0;
	}
	std::thread Start() {
		std::thread processThread(&SessionManager<T>::Process, this);

		return processThread;
	}

	T* CreateSession() {
		UINT32 sessionID = 0;
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
	void RemoveSession(UINT32 sessionID) {
		lock_guard<std::mutex> guard(_mutex);
		_onClosed.push(sessionID);
		_sessionCnt--;
	}
	void Close() {
		_isClosed = true;
	}

	void RegisterSession(T* session) {
		lock_guard<std::mutex> guard(m_register);

		registWait.push_back(session);
	}

protected:
	void AddSession(int count) {
		UINT32 idx = _sessions.size();
		for (UINT32 i = idx; i < idx + count; i++) {
			_sessions.push_back(new T(i));
			_onClosed.push(i);
		}
	}
	void Process() {
		while (!_isClosed) {
			if (registWait.empty() == false) {
				lock_guard<mutex> guard(m_register);

				while (registWait.empty() == false) {
					T* session = registWait.back();
					registWait.pop_back();
					session->Recv();
				}
			}

			SleepEx(0, TRUE);
		}
	}

protected:
	vector<T*> _sessions;
	priority_queue<UINT32, vector<UINT32>, greater<UINT32>> _onClosed;
	atomic<bool> _isClosed = false;
	std::mutex m_register;
	vector<T*> registWait;
};

