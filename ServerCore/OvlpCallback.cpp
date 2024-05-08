#include "pch.h"
#include "Session.h"
#include "OvlpCallback.h"
#include "SessionManager.h"


OvlpCallback::OvlpCallback() {

}

OvlpCallback::~OvlpCallback() {

}

int OvlpCallback::Start() {
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error");
		return -1;
	}

	_localSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_localSock == INVALID_SOCKET) {
		ErrorHandling("WSASocket() error");
		return -1;
	}

	int optval = 1;

	::setsockopt(_localSock, SOL_SOCKET, SO_REUSEADDR, (const char*) & optval, sizeof(optval));

	return 0;
}

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

void OvlpCallback::Close() {
	_isClosed = true;

	if(_localSock != 0)
		closesocket(_localSock);
	WSACleanup();
}

void OvlpCallback::ErrorHandling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
}