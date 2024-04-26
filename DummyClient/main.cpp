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
	{
		WSADATA wsaData;
		SOCKET hSock;
		SOCKADDR_IN serverAdr;
		LPWSAOVERLAPPED lpOvlp;
		DWORD recvBytes;
		LPPER_IO_DATA hbInfo;
		DWORD mode = 1, flags = 0;
		int recvAdrSz;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return -1;

		hSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (hSock == INVALID_SOCKET)
			return -1;
		memset(&serverAdr, 0, sizeof(SOCKADDR_IN));
		serverAdr.sin_family = PF_INET;
		serverAdr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		serverAdr.sin_port = htons(9190);

		if (::connect(hSock, (SOCKADDR*)&serverAdr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
			return -1;
		}

		puts("server connected....");

		lpOvlp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(lpOvlp, 0, sizeof(WSAOVERLAPPED));

		hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(hbInfo, 0, sizeof(PER_IO_DATA));
		hbInfo->hServSock = hSock;		// ¿Ö DWORD·Î ÁÜ?

		while (true) {
			TestPacket packet;
			packet.data = 1;
			int size = packet.Write(hbInfo->buf);

			(hbInfo->wsaBuf).buf = hbInfo->buf;
			hbInfo->wsaBuf.len = size;

			lpOvlp->hEvent = (HANDLE)hbInfo;
			WSASend(hSock, &(hbInfo->wsaBuf), 1, &recvBytes, 0, lpOvlp, SendCompRoutine);

			Sleep(1);
			memset(hbInfo->buf, 0, 1024);

			TestPacket packet2;
			packet2.data = 1000;
			size = packet2.Write(hbInfo->buf);

			(hbInfo->wsaBuf).buf = hbInfo->buf;
			hbInfo->wsaBuf.len = size;

			lpOvlp->hEvent = (HANDLE)hbInfo;
			WSASend(hSock, &(hbInfo->wsaBuf), 1, &recvBytes, 0, lpOvlp, SendCompRoutine);
			Sleep(1);
		}

			while (true);


		closesocket(hSock);
		WSACleanup();
	}

	//SessionManager<ServerSession>::GetInstance().Init();
	//std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

	//if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
	//	return -1;

	//ServerSession* session = OvlpCallback::GetInstance().Connect<ServerSession>(PF_INET, "127.0.0.1", 9190);
	//if (session == nullptr)
	//	return -1;

	//TestPacket packet;
	//packet.data = 10;

	//std::string input;

	//while (true) {
	//	Sleep(100);

	//	session->Send<TestPacket>(packet);
	//}

	//OvlpCallback::GetInstance().Close();
	//PacketQueue::GetInstance().Close();

	//packetFetchLoop.join();

	//std::cout << "Server Closed" << std::endl;

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