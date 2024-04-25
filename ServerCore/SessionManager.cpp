#include "pch.h"
#include "SessionManager.h"

SessionManager::SessionManager() : _sessionCnt(INIT_SESSION_COUNT) {
    for (int i = 1; i <= INIT_SESSION_COUNT; i++) {
        _sessions[i] = make_shared<Session>(i);
        _onClosed.push(i);
    }

    _sessionCnt = 0;
}

SessionManager::~SessionManager() {
    _sessions.clear();
}

Session* SessionManager::CreateSession() {
    int sessionID = -1;
    {
        lock_guard<std::mutex> guard(_mutex);
        sessionID = _onClosed.top();
        _onClosed.pop();

        if (sessionID == -1) {
            int idx = _sessions.size();
            for (int i = 0; i < 5; i++) {
                _sessions[idx + i] = make_shared<Session>(idx + i);
                if (i != 0)
                    _onClosed.push(idx + i);
            }

            sessionID = idx;
        }
    }
    
    _sessionCnt++;
    return _sessions[sessionID].get();
}

void SessionManager::RemoveSession(int sessionID) {
    lock_guard<std::mutex> guard(_mutex);
    _onClosed.push(sessionID);
    _sessionCnt--;
}
