#pragma once
#include "pch.h"
#include "Defines.h"
#include "Session.h"
#include "SessionManager.h"

#define BUF_SIZE 1024

class OvlpCallback {
	SINGLETON(OvlpCallback);

private:
	//typedef struct {
	//	SOCKET hSocket;
	//	char buf[BUF_SIZE];
	//	WSABUF wsaBuf;
	//	DWORD sentBytes, recvBytes, flags;
	//}PER_IO_DATA, * LPPER_IO_DATA;
	WSADATA _wsaData;
	bool _isClosed = false;

public:
	int Start();


	int BindnListen(std::string ipAddress = "127.0.0.1",
					USHORT port = 9190,
					int backlog = 5);


	template <typename T = Session>
	void AcceptLoop() {
		while (!_isClosed) {
			try{
				_remoteAdrSz = sizeof(SOCKADDR_IN);
				memset(&_remoteAdr, 0, sizeof(SOCKADDR_IN));

				while (true) {
					_remoteSock = ::accept(_localSock, (SOCKADDR*)&_remoteAdr, &_remoteAdrSz);
					if (_remoteSock == INVALID_SOCKET) {
						if (WSAGetLastError() != WSAEWOULDBLOCK)
							ErrorHandling("::accept() error");
						else
							continue;
					}
					else
						break;
				}

			} catch (std::exception e) {
				cout << e.what() << '\n';
				continue;
			}

			puts("client connected");

			T* session = SessionManager<T>::GetInstance().CreateSession();

			session->Initialize(_remoteSock, _remoteAdr);
			SessionManager<T>::GetInstance().RegisterSession(session);
		}
	}

	template <typename T = Session>
	T* Connect(ADDRESS_FAMILY family,
				std::string remoteIpAddress,
				USHORT rmPort = 9190) {
		memset(&_remoteAdr, 0, sizeof(_remoteAdr));
		_remoteAdr.sin_family = family;
		_remoteAdr.sin_addr.S_un.S_addr = inet_addr(remoteIpAddress.c_str());
		_remoteAdr.sin_port = htons(rmPort);
		//TODO: 이후에 개인 욕심으로 한번 시도해볼만한 것 -> AcceptEx도 포함.
		//int errCode = WSAConnect(_localSock, (SOCKADDR*)&_remoteAdr, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL);
		
		if (::connect(_localSock, (SOCKADDR*)&_remoteAdr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
			ErrorHandling("::connect() error");
			return nullptr;
		}

		u_long mode = 1;
		int optval = 1;

		//::setsockopt(_localSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
		::ioctlsocket(_localSock, FIONBIO, &mode);

		T* session = nullptr;
		session = SessionManager<T>::GetInstance().CreateSession();
		session->Initialize(_localSock, _remoteAdr);

		return session;
	}


	void Close();

public:
	void ErrorHandling(const char* message);

private:
	SOCKET _localSock = 0, _remoteSock = 0;
	SOCKADDR_IN _localAdr, _remoteAdr;
	int _remoteAdrSz;
};

