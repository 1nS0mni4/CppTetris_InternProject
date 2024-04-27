#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "ServerSession.h"
#include "OvlpCallback.h"
#include "PacketQueue.h"
#include <WinSock2.h>

void ErrorHandling(const char* message);

static ServerSession* session = nullptr;


typedef struct {
	SOCKET hServSock;
	char buf[1024];
	WSABUF wsaBuf;
}PER_IO_DATA, * LPPER_IO_DATA;

void CALLBACK RecvCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK SendCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

int main(int argc, char* argv[]) {
	SessionManager<ServerSession>::GetInstance().Init();
	std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

	if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
		return -1;

	ServerSession* session = OvlpCallback::GetInstance().Connect<ServerSession>(PF_INET, "127.0.0.1", 9190);
	if (session == nullptr)
		return -1;

	TestPacket* packet = new TestPacket();
	int i = 1;
	packet->data = i;

	std::string input;

	while (true) {
		session->Send<TestPacket>(packet);
		Sleep(10);
		new (packet) TestPacket;
		packet->data = ++i;
	}

	OvlpCallback::GetInstance().Close();
	PacketQueue::GetInstance().Close();

	packetFetchLoop.join();

	std::cout << "Server Closed" << std::endl;

	return 0;
}

void CALLBACK RecvCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
	SOCKET hSock = hbInfo->hServSock;
	LPWSABUF bufInfo = &(hbInfo->wsaBuf);
	DWORD sentBytes;

	if (szRecvBytes == 0) {
		closesocket(hSock);
		free(lpOverlapped->hEvent); free(lpOverlapped);

		puts("client disconnected....");
	}
	else {
		cout << bufInfo->buf << '\n';
	}
}

void CALLBACK SendCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
	LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
	SOCKET hSock = hbInfo->hServSock;
	LPWSABUF bufInfo = &(hbInfo->wsaBuf);
	DWORD recvBytes;

	DWORD flagInfo = 0;
	//WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, RecvCompRoutine);
}

void ErrorHandling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(-1);
}