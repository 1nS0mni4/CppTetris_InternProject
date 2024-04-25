#pragma once
#include "pch.h"
#include "Session.h"
#include "Session.h"
#include "SessionManager.h"
#include "ServerPacketHandler.h"

class ClientSession : public Session {
public:
	ClientSession(int sessionID) : Session(sessionID) {

	}

	virtual ~ClientSession() override {

	}

	virtual void Disconnect() override;

public:
	virtual void OnSend() override;
	virtual void OnRecv(Packet packet, USHORT packetID) override;
	virtual void OnConnect() override;
	virtual void OnDisconnect() override;
};

using ClientSessionManager = SessionManager<ClientSession>;
