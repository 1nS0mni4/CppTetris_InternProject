#include "pch.h"
#include "Session.h"
#include "Packet.h"
#include "OvlpCallback.h"
#include "SendBuffer.h"
#include "PacketHandler.h"
#include "SessionManager.h"

Session::Session(int sessionID) 
	: _sessionID(sessionID),
	sendOvlp(new WSAOVERLAPPED()),
	recvOvlp(new WSAOVERLAPPED()) {
	memset(sendOvlp, 0, sizeof(WSAOVERLAPPED));
	memset(recvOvlp, 0, sizeof(WSAOVERLAPPED));

	{
		recvBuf = (char*)malloc(SESSION_RECVBUF_SIZE);
		memset(recvBuf, 0, sizeof(SESSION_RECVBUF_SIZE));
		_read = _write = 0;
		wsaRecvBuf.buf = recvBuf;
		wsaRecvBuf.len = SESSION_RECVBUF_SIZE;
	}

	sendInfo.session = recvInfo.session = this;
	sendInfo.wsaBuf = &wsaSendBuf;
	recvInfo.wsaBuf = &wsaRecvBuf;

	sendOvlp->hEvent = (HANDLE)&sendInfo;
	recvOvlp->hEvent = (HANDLE)&recvInfo;
}

Session::~Session() {
	delete sendOvlp;
	delete recvOvlp;
	free(recvBuf);
}

void Session::Init(SOCKET socket, SOCKADDR_IN remoteAdr) {
	_socket = socket;
	_remoteAdr = remoteAdr;

	sentBytes = recvBytes = flags = 0;
}

void Session::Disconnect() {
	shutdown(_socket, SD_BOTH);
	closesocket(_socket);
	Clear();
}

void Session::Clear() {
	_socket = INVALID_SOCKET;
	memset(&_remoteAdr, 0, sizeof(_remoteAdr));
	memset(recvBuf, 0, SESSION_RECVBUF_SIZE);
	_read = _write = 0;
}

void Session::SendSegment() {
	{
		lock_guard<std::mutex> guard(sendMtx);
		if (sPending.empty())
			return;

		wsaSendBuf = sPending.front();
		sPending.pop();
	}

	int result = ::WSASend(_socket, &wsaSendBuf, 1, &sentBytes, 0, sendOvlp, SendCompRoutine);
	if (result == SOCKET_ERROR) {
		result = WSAGetLastError();
		if (result == WSA_IO_PENDING) {
			cout << "Send : IO Pending\n";
		}
	}
}

void CALLBACK Session::SendCompRoutine(DWORD dwError, DWORD szSentBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	SendInfo* info = (SendInfo*)(lpOverlapped->hEvent);
	Session* session = info->session;
	LPWSABUF buf = info->wsaBuf;

	SendBuffer::GetInstance().setUsed(buf->len);
	session->SendSegment();
}

void Session::Recv() {
	if (recvAtm.exchange(true))
		return;
	
	OrganizeRecvBuf();

	int result = ::WSARecv(_socket, &wsaRecvBuf, 1, &recvBytes, &flags, recvOvlp, RecvCompRoutine);
	if (result == SOCKET_ERROR) {
		result = WSAGetLastError();
		if (result == WSA_IO_PENDING)
			cout << "Recv : IO Pending\n";
	}
}

void CALLBACK Session::RecvCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	lpRecvInfo info = (lpRecvInfo)(lpOverlapped->hEvent);
	Session* session = info->session;
	LPWSABUF bufInfo = info->wsaBuf;

	if (szRecvBytes == 0) {
		puts("Client disconnected...");
		return;
	} 

	session->_write += szRecvBytes;
	    
	session->_read = session->OnRecv(bufInfo->buf, szRecvBytes);

	session->OrganizeRecvBuf();
	session->recvAtm.exchange(false);
	session->Recv();
}

void Session::OrganizeRecvBuf() {
	if (_write != _read) {
		strcpy_s(recvBuf, SESSION_RECVBUF_SIZE, recvBuf + _read);
		_write -= _read;
		_read = 0;
		return;
	}

	_read = _write = 0;
}