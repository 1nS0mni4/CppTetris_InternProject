#include "pch.h"
#include "Session.h"


Session::Session(UINT32 sessionID) 
	: _sessionID(sessionID) {
	sendOvlp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
	recvOvlp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
	sendInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
	recvInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
	sendInfo->session = this;
	sendInfo->wsaBuf.buf = sendInfo->buf;
	sendInfo->wsaBuf.len = SENDBUF_SIZE;

	recvInfo->session = this;
	recvInfo->wsaBuf.buf = recvInfo->buf;
	recvInfo->wsaBuf.len = RECVBUF_SIZE;

	sendOvlp->hEvent = (HANDLE)sendInfo;
	recvOvlp->hEvent = (HANDLE)recvInfo;
}

Session::~Session() {
	free(sendOvlp);
	free(recvOvlp);
	free(sendInfo);
	free(recvInfo);
}

void Session::Initialize(SOCKET socket, SOCKADDR_IN remoteAdr) {
	_socket = socket;
	_remoteAdr = remoteAdr;

	_rcRead = _rcWrite = 0;
	sentBytes = recvBytes = flags = 0;
}

void Session::Disconnect() {
	OnDisconnect();
	shutdown(_socket, SD_BOTH);
	closesocket(_socket);
	Clear();
}

void Session::Clear() {
	_socket = INVALID_SOCKET;
	memset(&_remoteAdr, 0, sizeof(_remoteAdr));
	_rcRead = _rcWrite = 0;
	recvAtm = false;
}

int Session::SendSegment() {
	int processed = 0;
	{
		lock_guard<std::mutex> guard(pendMtx);

		if (sPending.empty())
			return 0;

		while (sPending.empty() == false) {
			sending.push_back(sPending.back());
			sPending.pop_back();
		}
	}
	{
		lock_guard<std::mutex> guard(sendMtx);
		char* startPos = sendInfo->buf;
		while (sending.empty() == false) {
			Packet* packet = sending.back().get();

			int size = packet->Write(startPos);
			startPos += size;
			processed += size;
			sending.pop_back();
		}
		sendInfo->wsaBuf.len = processed;

		int result = ::WSASend(_socket, &(sendInfo->wsaBuf), 1, &sentBytes, 0, sendOvlp, SendCompRoutine);
		result = WSAGetLastError();
		result;
		return processed;
	}
}

void CALLBACK Session::SendCompRoutine(DWORD dwError, DWORD szSentBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	LPPER_IO_DATA info = (LPPER_IO_DATA)(lpOverlapped->hEvent);
	Session* session = info->session;
	WSABUF buf = info->wsaBuf;

	info->wsaBuf.len = 0;
	session->SendSegment();
}

void Session::Recv() {
	if (recvAtm.exchange(true))
		return;

	int result = ::WSARecv(_socket, &(recvInfo->wsaBuf), 1, &recvBytes, &flags, recvOvlp, RecvCompRoutine);
	result = WSAGetLastError();
	result;
}

void CALLBACK Session::RecvCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	LPPER_IO_DATA info = (LPPER_IO_DATA)(lpOverlapped->hEvent);
	Session* session = info->session;
	WSABUF bufInfo = info->wsaBuf;

	if (szRecvBytes == 0) {
		puts("Client disconnected...");
		bool success = CancelIo((HANDLE)session->GetSocket());
		session->Disconnect();
		return;
	} 
	session->_rcWrite += szRecvBytes;
	session->OnRecv(bufInfo.buf, szRecvBytes);
	session->OrganizeRecvBuf();
	session->recvAtm.exchange(false);

	session->Recv();
}

void Session::OrganizeRecvBuf() {
	if (_rcWrite != _rcRead) {
		strcpy_s(recvInfo->buf, RECVBUF_SIZE, &recvInfo->buf[_rcRead]);
		_rcWrite -= _rcRead;
		_rcRead = 0;
		return;
	}

	_rcRead = _rcWrite = 0;
}

void Session::OrganizeSendBuf() {
	if (_sdWrite != _sdUsed) {
		strcpy_s(sendInfo->buf, SENDBUF_SIZE, &(sendInfo->buf[_sdUsed]));
		_sdWrite -= _sdUsed;
		_sdUsed = 0;
		return;
	}

	_sdUsed = _sdWrite = 0;
}