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
					USHORT port = 9190,
					int backlog = 5);


	template <typename T = Session>
	void AcceptLoop() {
		while (!_isClosed) {
			int recvAdrSz = sizeof(_remoteAdr);

			_remoteSock = ::accept(_localSock, (SOCKADDR*)&_remoteAdr, &recvAdrSz);
			if (_remoteSock == INVALID_SOCKET) {
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					ErrorHandling("::accept() error");

				continue;
			}

			puts("client connected");

			T* session = SessionManager<T>::GetInstance().CreateSession();

			session->Init(_remoteSock, _remoteAdr);
			session->Recv();
		}
	}

	template <typename T = Session>
	T* Connect(ADDRESS_FAMILY family,
				std::string remoteIpAddress,
				USHORT rmPort) {
		memset(&_remoteAdr, 0, sizeof(_remoteAdr));
		_remoteAdr.sin_family = family;
		_remoteAdr.sin_addr.S_un.S_addr = inet_addr(remoteIpAddress.c_str());
		_remoteAdr.sin_port = htons(rmPort);

		if (::connect(_localSock, (SOCKADDR*)&_remoteAdr, sizeof(_remoteAdr)) == SOCKET_ERROR) {
			ErrorHandling("::connect() error");
			return nullptr;
		}


		T* session = SessionManager<T>::GetInstance().CreateSession();
		session->Init(_localSock, _remoteAdr);
		session->Recv();

		return session;
	}


	void Close();

public:

	void ErrorHandling(const char* message);

private:
	SOCKET _localSock, _remoteSock;
	SOCKADDR_IN _localAdr, _remoteAdr;

};

