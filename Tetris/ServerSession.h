#pragma once
#include "CorePch.h"
#include "Session.h"
#include "SessionManager.h"

class ServerSession : public Session {
public:
	ServerSession(int sessionID) : Session(sessionID) {

	}

	virtual ~ServerSession() {

	}

	virtual void Disconnect() override;

public:
	virtual void OnSend() override;
	virtual int OnRecv(char* packet, int size) override;
	virtual void OnConnect() override;
	virtual void OnDisconnect() override;

public:
	wchar_t name[NAME_LEN];
};

using ServerSessionManager = SessionManager<ServerSession>;