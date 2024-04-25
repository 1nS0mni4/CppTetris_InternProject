#pragma once
#include "pch.h"
#include "Defines.h"
#include "CorePch.h"

#define BUF_SIZE 1024

class OvlpCallback {
	SINGLETON(OvlpCallback);

protected:
	class OvlpPool {	
	private:
		std::queue<LPWSAOVERLAPPED> _ovlpPool;
		//TODO: vector로 바꾸는게 나을듯

	public:
		OvlpPool(int poolCnt) {

		}
		~OvlpPool() {

		}

		LPWSAOVERLAPPED get() {

		}

		void release(LPWSAOVERLAPPED released) {

		}

	};

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

#ifdef _SERVER_
	int BindnListen(std::string ipAddress,
					USHORT port,
					int backlog = 5);
	void AcceptLoop();

#endif

#ifdef _CLIENT_
	int Connect(ADDRESS_FAMILY family,
				std::string remoteIpAddress,
				USHORT rmPort);
#endif

	void Close();

public:

	void ErrorHandling(const char* message);

private:
	SOCKET _localSock, _remoteSock;
	SOCKADDR_IN _localAdr, _remoteAdr;

};

