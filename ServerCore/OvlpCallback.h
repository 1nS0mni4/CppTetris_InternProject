#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"
#include "SessionManager.h"

#define BUF_SIZE 1024

class OvlpCallback {
	SINGLETON(OvlpCallback);

private:
	typedef struct {
		SOCKET hSocket;
		char buf[BUF_SIZE];
		WSABUF wsaBuf;
		DWORD sentBytes, recvBytes, flags;
	}PER_IO_DATA, * LPPER_IO_DATA;
	WSADATA _wsaData;
	bool _isClosed = false;

public:
	int Start();


	int BindnListen(std::string ipAddress,
					USHORT port,
					int backlog = 5);


	template <typename T = Session>
	void AcceptLoop() {
		while (!_isClosed) {
			int recvAdrSz = sizeof(_remoteAdr);

			SleepEx(100, TRUE);
			_remoteSock = ::accept(_localSock, (SOCKADDR*)&_remoteAdr, &recvAdrSz);
			if (_remoteSock == INVALID_SOCKET) {
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					ErrorHandling("::accept() error");

				continue;
			}

			puts("client connected");


			Session* session = SessionManager<T>::GetInstance().CreateSession();

			session->Init(_remoteSock, _remoteAdr);
			session->Recv();
		}
	}

	int Connect(ADDRESS_FAMILY family,
				std::string remoteIpAddress,
				USHORT rmPort);


	void Close();

public:

	void ErrorHandling(const char* message);

private:
	SOCKET _localSock, _remoteSock;
	SOCKADDR_IN _localAdr, _remoteAdr;

};

