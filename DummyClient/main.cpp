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
	int write, read;
	WSABUF wsaBuf;
}PER_IO_DATA, * LPPER_IO_DATA;

void CALLBACK RecvCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK SendCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

int main(int argc, char* argv[]) {
	//WSADATA wsaData;
	//SOCKET hSocket;
	//SOCKADDR_IN sendAdr;

	//WSABUF dataBuf;
	//char msg[] = "network is computer";
	//DWORD sendBytes = 0;

	//WSAEVENT evObj;
	//WSAOVERLAPPED overlapped;

	//if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	//	ErrorHandling("WSAStartup() error");

	//hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	//memset(&sendAdr, 0, sizeof(sendAdr));
	//sendAdr.sin_family = AF_INET;
	//sendAdr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//sendAdr.sin_port = htons(9190);

	//if (connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
	//	ErrorHandling("connect() error");

	//evObj = WSACreateEvent();
	//memset(&overlapped, 0, sizeof(overlapped));
	//overlapped.hEvent = evObj;
	//dataBuf.len = strlen(msg) + 1;
	//dataBuf.buf = msg;

	//TestPacket packet;
	//packet.data = 10;
	//char* segment = new char[8];
	//int size = packet.Write(segment);
	//dataBuf.buf = segment;
	//dataBuf.len = size;

	//while (true) {
	//	if (WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR) {
	//		if (WSAGetLastError() == WSA_IO_PENDING) {
	//			puts("Background data send");
	//			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
	//			WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
	//		}
	//		else {
	//			ErrorHandling("WSASend() error");
	//		}
	//	}

	//	printf("Send data size: %d\n", sendBytes);
	//	SleepEx(1000, FALSE);
	//}

	//WSACloseEvent(evObj);
	//closesocket(hSocket);
	//WSACleanup();

	//return 0;

	vector<std::thread> threads;
	srand(time(NULL));

	for (int i = 0; i < 10; i++) {
		threads.push_back(thread([]() {
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

			lpOvlp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
			memset(lpOvlp, 0, sizeof(WSAOVERLAPPED));

			hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(hbInfo, 0, sizeof(PER_IO_DATA));
			hbInfo->hServSock = hSock;		// ¿Ö DWORD·Î ÁÜ?

			TestPacket packet;
			packet.data = rand() % 100;
			int size = packet.Write(hbInfo->buf, hbInfo->write);
			hbInfo->write += size;
			(hbInfo->wsaBuf).buf = hbInfo->buf;
			hbInfo->wsaBuf.len += size;

			lpOvlp->hEvent = (HANDLE)hbInfo;

			puts("server connected....");

			while (true) {
				Sleep(10);
				WSASend(hSock, &(hbInfo->wsaBuf), 1, &recvBytes, 0, lpOvlp, SendCompRoutine);
			}

			closesocket(hSock);
			WSACleanup();
						  }));
		threads[i].join();
	}

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

/*

*/


void ErrorHandling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(-1);
}