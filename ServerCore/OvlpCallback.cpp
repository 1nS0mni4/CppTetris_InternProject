#include "pch.h"
#include "Session.h"
#include "OvlpCallback.h"

OvlpCallback::OvlpCallback() {
	//for(int i = 0; i < 100; i++){
	//	_ovlpPool.push(new WSAOVERLAPPED);
	//}
	//TODO: 아마 싱글턴이 나을듯.
}

OvlpCallback::~OvlpCallback() {
	//while (_ovlpPool.empty() != false) {
	//	LPWSAOVERLAPPED ovlp = _ovlpPool.front();
	//	_ovlpPool.pop();
	//	delete ovlp;
	//}
}

int OvlpCallback::Start() {
	WSADATA wsaData;
	LPWSAOVERLAPPED lpOvlp;
	DWORD recvBytes = 0, flags = 0;
	LPPER_IO_DATA hbInfo;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error");
		return -1;
	}

	_localSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_localSock == INVALID_SOCKET) {
		ErrorHandling("WSASocket() error");
		return -1;
	}

	return 0;
}
#ifdef _SERVER_
int OvlpCallback::BindnListen(std::string ipAddress, USHORT port, int backlog) {
	memset(&_localAdr, 0, sizeof(_localAdr));
	_localAdr.sin_family = AF_INET;
	_localAdr.sin_addr.S_un.S_addr = inet_addr(ipAddress.c_str());
	_localAdr.sin_port = htons(port);

	if (::bind(_localSock, (SOCKADDR*)&_localAdr, sizeof(_localAdr)) == SOCKET_ERROR) {
		ErrorHandling("::bind() error");
		return -1;
	}

	if (::listen(_localSock, backlog) == SOCKET_ERROR) {
		ErrorHandling("::listen() error");
		return -1;
	}

	return 0;
}
#endif

#ifdef _CLIENT_
int OvlpCallback::Connect(ADDRESS_FAMILY family, std::string remoteIpAddress, USHORT rmPort) {
	memset(&_remoteAdr, 0, sizeof(_remoteAdr));
	_remoteAdr.sin_family = family;
	_remoteAdr.sin_addr.S_un.S_addr = inet_addr(remoteIpAddress.c_str());
	_remoteAdr.sin_port = htons(rmPort);

	if (::connect(_localSock, (SOCKADDR*)&_remoteSock, sizeof(_remoteSock)) == SOCKET_ERROR) {
		ErrorHandling("::connect() error");
		return -1;
	}

	return 0;
}
#endif
void OvlpCallback::AcceptLoop() {
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

		LPWSAOVERLAPPED lpOvlp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(lpOvlp, 0, sizeof(WSAOVERLAPPED));

		LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		hbInfo->hSocket = _remoteSock;
		(hbInfo->wsaBuf).buf = hbInfo->buf;
		(hbInfo->wsaBuf).len = BUF_SIZE;
		hbInfo->recvBytes = hbInfo->sentBytes = 0;
		hbInfo->flags = 0;
		
		lpOvlp->hEvent = (HANDLE)hbInfo;
		WSARecv(_remoteSock, &(hbInfo->wsaBuf), 1, &(hbInfo->recvBytes), &(hbInfo->flags), lpOvlp, RecvCompRoutine);
	}
}

void OvlpCallback::Close() {
	_isClosed = true;
	closesocket(_remoteSock);
	closesocket(_localSock);
	WSACleanup();
}

void CALLBACK OvlpCallback::RecvCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
	SOCKET hSock = hbInfo->hSocket;
	LPWSABUF bufInfo = &(hbInfo->wsaBuf);
	DWORD sendBytes;

	if (szRecvBytes == 0) {
		closesocket(hSock);
		free(lpOverlapped->hEvent); free(lpOverlapped);
		puts("Client disconnected...");
	}
	else {
		bufInfo->len = szRecvBytes;

		//TODO: 처리 루틴 추가

		std::cout << "Received: " << bufInfo->buf << '\n';

		//WSASend(hSock, bufInfo, 1, &sendBytes, 0, lpOverlapped, SendCompRoutine);
		WSARecv(hSock, bufInfo, 1, &(hbInfo->recvBytes), &(hbInfo->flags), lpOverlapped, RecvCompRoutine);
		//TODO: Session으로 옮기기
	}
}

void CALLBACK OvlpCallback::SendCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
	SOCKET hSock = hbInfo->hSocket;
	LPWSABUF bufInfo = &(hbInfo->wsaBuf);
	DWORD recvBytes;
	DWORD flagInfo = 0;

	WSARecv(hSock, bufInfo, 1, &(hbInfo->recvBytes), &(hbInfo->flags), lpOverlapped, RecvCompRoutine);

	//TODO: Session으로 옮기기
}

void OvlpCallback::ErrorHandling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
}