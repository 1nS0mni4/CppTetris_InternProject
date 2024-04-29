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

public:
	wchar_t name[NAME_LEN];
	atomic<bool> isLogined;
	atomic<bool> isRunning;
	mutex m_piece;
	mutex m_score;
	mutex m_field;
};

using ServerSessionManager = SessionManager<ServerSession>;