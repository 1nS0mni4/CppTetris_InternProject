#include "pch.h"
#include "Session.h"


Session::Session(int sessionID) 
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
	shutdown(_socket, SD_BOTH);
	closesocket(_socket);
	Clear();
}

void Session::Clear() {
	_socket = INVALID_SOCKET;
	memset(&_remoteAdr, 0, sizeof(_remoteAdr));
	_rcRead = _rcWrite = 0;
}

void Session::SendSegment() {
	{
		lock_guard<std::mutex> guard(sendMtx);
		if (sPending.empty())
			return;

		//TODO: 여기도 벡터 두개로 쌓인거 한번에 몰아서 보내면 좋을듯
		//TODO: sendInfo->buf에 쌓인 패킷 순서대로 오프셋줘서 덮어쓰기 한 담에 len +=해주면 될듯

		//Packet* packet;// = sPending.back
		//int size = packet->Write(sendInfo->buf);
		//sendInfo->wsaBuf.len = size;
	}

	int result = ::WSASend(_socket, &(sendInfo->wsaBuf), 1, &sentBytes, 0, sendOvlp, SendCompRoutine);
	result = WSAGetLastError();
	result;
}

void CALLBACK Session::SendCompRoutine(DWORD dwError, DWORD szSentBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	LPPER_IO_DATA info = (LPPER_IO_DATA)(lpOverlapped->hEvent);
	Session* session = info->session;
	WSABUF buf = info->wsaBuf;

	SendBuffer::GetInstance().setUsed(szSentBytes);
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
		return;
	} 

	session->OnRecv(bufInfo.buf, szRecvBytes);
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