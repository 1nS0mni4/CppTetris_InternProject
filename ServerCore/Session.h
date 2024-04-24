#pragma once
#include "pch.h"

class Session {
public:
	Session(int sessionID);
	virtual ~Session();

public:
	UINT32 getSessionID() { return _sessionID; }
	SOCKET getSocket() { return _socket; }

public:
	void Init(SOCKET socket, SOCKADDR_IN remoteAdr);
	int Send();
	int Recv();

private:
	UINT32 _sessionID;
	SOCKET _socket;
	SOCKADDR_IN _remoteAdr;

public:
	WSABUF recvBuf, sendBuf;
	DWORD sentBytes, recvBytes, flags;


};