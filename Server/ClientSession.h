#pragma once
#include "pch.h"
#include "Session.h"
#include "SessionManager.h"

class ClientSession : public Session {
public:
	ClientSession(int sessionID) : Session(sessionID) {

	}

	virtual ~ClientSession() override {

	}

	virtual void Disconnect() override;
};

