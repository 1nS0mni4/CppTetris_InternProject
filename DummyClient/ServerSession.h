#pragma once
#include "pch.h"
#include "Session.h"
#include "SessionManager.h"
#include "ClientPacketHandler.h"

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
};

using ServerSessionManager = SessionManager<ServerSession>;