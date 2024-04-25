#include "pch.h"
#include "Session.h"
#include "Packet.h"
#include "OvlpCallback.h"
#include "SendBuffer.h"
#include "PacketHandler.h"
#include "SessionManager.h"

Session::Session(int sessionID) 
	: _sessionID(sessionID),
	sendOvlp(new WSAOVERLAPPED),
	recvOvlp(new WSAOVERLAPPED) {
	memset(sendOvlp, 0, sizeof(WSAOVERLAPPED));
	memset(recvOvlp, 0, sizeof(WSAOVERLAPPED));

	sendOvlp->hEvent = (HANDLE)&sendInfo;
	recvOvlp->hEvent = (HANDLE)&recvInfo;

	sendInfo.session = recvInfo.session = this;
	sendInfo.wsaBuf = &wsaSendBuf;
	recvInfo.wsaBuf = &wsaRecvBuf;
	recvBuf = (char*)malloc(SESSION_RECVBUF_SIZE);
	_read = _write = 0;
	wsaRecvBuf.buf = recvBuf;
	wsaRecvBuf.len = SESSION_RECVBUF_SIZE;
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

	::WSASend(_socket, &wsaSendBuf, 1, &sentBytes, 0, sendOvlp, SendCompRoutine);
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
	
	OrganizeRecvBuf(_read, _write);

	::WSARecv(_socket, &wsaSendBuf, 1, &recvBytes, 0, recvOvlp, RecvCompRoutine);
}

void CALLBACK Session::RecvCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	lpRecvInfo info = (lpRecvInfo)(lpOverlapped->hEvent);
	Session* session = info->session;
	LPWSABUF bufInfo = info->wsaBuf;

	if (szRecvBytes == 0) {
		puts("Client disconnected...");
		return;
	}

	int offset = 0;
	int size = 0;
	while (szRecvBytes <= 4) {
		Packet packet;
		Segment segment = &((bufInfo->buf)[offset]);
		size = packet.Read(segment);

		PacketHandler::GetInstance().HandlePacket(session, &packet, packet.packetID());
		
		session->OnRecv(packet, packet.size());

		std::cout << "Received: " << bufInfo->buf << '\n';
		szRecvBytes -= size;
		offset += size;
	}

	session->OrganizeRecvBuf(offset, szRecvBytes - size);
	session->recvAtm.exchange(false);
	session->Recv();
}

void Session::OrganizeRecvBuf(int offset, int size) {
	if (offset != size) {
		strcpy_s(recvBuf, SESSION_RECVBUF_SIZE, recvBuf + offset);
		_write = size;
		_read = 0;
		return;
	}

	_read = _write = 0;
}

//WSASend(hSock, bufInfo, 1, &sendBytes, 0, lpOverlapped, SendCompRoutine);
//WSARecv(hSock, bufInfo, 1, &(hbInfo->recvBytes), &(hbInfo->flags), lpOverlapped, RecvCompRoutine);