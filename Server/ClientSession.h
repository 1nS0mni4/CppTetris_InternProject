#pragma once
#include "pch.h"
#include "Session.h"
#include "SessionManager.h"
#include "Room.h"

class ClientSession : public Session {
public:
	ClientSession(UINT32 sessionID) : 
		Session(sessionID) {
		_room = nullptr;
	}

	virtual ~ClientSession() override {

	}

	virtual void Disconnect() override;

public:
	virtual void OnSend() override;
	virtual int OnRecv(char* packet, int size) override;
	virtual void OnConnect() override;
	virtual void OnDisconnect() override;

public:
	wchar_t name[NAME_LEN];
	Room* _room;
};

using ClientSessionManager = SessionManager<ClientSession>;
