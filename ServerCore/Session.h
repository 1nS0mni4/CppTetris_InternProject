#pragma once
#include "pch.h"
#include "Packet.h"
#include "OvlpCallback.h"

class Session {
public:
	Session(int sessionID);
	virtual ~Session();

public:
	UINT32 getSessionID() { return _sessionID; }
	SOCKET getSocket() { return _socket; }

public:
	void Init(SOCKET socket, SOCKADDR_IN remoteAdr);
	int Send(Packet packet);
	int Recv();

private:
	UINT32 _sessionID;
	SOCKADDR_IN _remoteAdr;
	SOCKET _socket;

public:
	DWORD sentBytes, recvBytes, flags;
	WSABUF recvBuf, sendBuf;
	


};