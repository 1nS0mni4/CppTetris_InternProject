#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "ClientSession.h"
#include "OvlpCallback.h"
#include "PacketQueue.h"

//void CALLBACK RecvCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
//void CALLBACK SendCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

typedef struct {
	SOCKET hClntSock;
	char buf[1024];
	WSABUF wsaBuf;
}PER_IO_DATA, *LPPER_IO_DATA;

int main(int argc, char* argv[]) {
	SessionManager<ClientSession>::GetInstance().Init();
	std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

	if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
		return -1;

	if (OvlpCallback::GetInstance().BindnListen("127.0.0.1", 9190) == SOCKET_ERROR)
		return -1;

	
	std::thread acceptLoop(&OvlpCallback::AcceptLoop<ClientSession>, &OvlpCallback::GetInstance());
	
	std::string input;

	while (true) {
		//std::cin >> input;

		//if (strcmp(input.c_str(), "quit") == 0 || strcmp(input.c_str(), "Quit") == 0)
		//	break;

		//TODO: 콘솔 커맨드 기능 추가
	}

	OvlpCallback::GetInstance().Close();
	PacketQueue::GetInstance().Close();

	acceptLoop.join();
	packetFetchLoop.join();

	std::cout << "Server Closed" << std::endl;

	//WSADATA wsaData;
	//SOCKET hLisnSock, hRecvSock;
	//SOCKADDR_IN lisnAdr, recvAdr;
	//LPWSAOVERLAPPED lpOvlp;
	//DWORD recvBytes;
	//LPPER_IO_DATA hbInfo;
	//DWORD mode = 1, flags = 0;
	//int recvAdrSz;

	//if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	//	return -1;
	//
	//hLisnSock = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	//if (hLisnSock == INVALID_SOCKET)
	//	return -1;

	//memset(&lisnAdr, 0, sizeof(SOCKADDR_IN));
	//lisnAdr.sin_family = AF_INET;
	//lisnAdr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//lisnAdr.sin_port = htons(9190);

	//if (::bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	//	return -1;
	//if (::listen(hLisnSock, 5) == SOCKET_ERROR)
	//	return -1;

	//recvAdrSz = sizeof(recvAdr);
	//SleepEx(100, TRUE);
	//hRecvSock = ::accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);

	//if (hRecvSock == INVALID_SOCKET) {
	//	if (WSAGetLastError() != WSAEWOULDBLOCK)
	//		return -1;
	//}


	//while (1) {
	//	puts("client connected....");
	//	lpOvlp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
	//	memset(lpOvlp, 0, sizeof(WSAOVERLAPPED));

	//	hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
	//	hbInfo->hClntSock = hRecvSock;		// 왜 DWORD로 줌?
	//	(hbInfo->wsaBuf).buf = hbInfo->buf;
	//	(hbInfo->wsaBuf).len = 1024;

	//	lpOvlp->hEvent = (HANDLE)hbInfo;
	//	WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flags, lpOvlp, RecvCompRoutine);
	//}

	//closesocket(hRecvSock);
	//closesocket(hLisnSock);
	//WSACleanup();

	return 0;
}

//void CALLBACK RecvCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
//	LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
//	SOCKET hSock = hbInfo->hClntSock;
//	LPWSABUF bufInfo = &(hbInfo->wsaBuf);
//	DWORD sentBytes;
//
//	if (szRecvBytes == 0) {
//		closesocket(hSock);
//		free(lpOverlapped->hEvent); free(lpOverlapped);
//
//		puts("client disconnected....");
//	}
//	else {
//		bufInfo->len = szRecvBytes;
//		puts("Echo!");
//		WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, SendCompRoutine);
//	}
//}
//
//
//void CALLBACK SendCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
//	LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)lpOverlapped->hEvent;
//	SOCKET hSock = hbInfo->hClntSock;
//	LPWSABUF bufInfo = &(hbInfo->wsaBuf);
//	DWORD recvBytes;
//
//	DWORD flagInfo = 0;
//	WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, RecvCompRoutine);
//
//}

/*
	//std::thread test1(&PacketQueue::Flush, &PacketQueue::GetInstance());

	//char* buf = new char[100];
	//memset(buf, 0, 100);
	//TestPacket* packet = new TestPacket();
	//packet->data = 10;
	//int size = packet->Write(buf);

	//while (true) {
	//	PacketQueue::GetInstance().Push(nullptr, buf, (USHORT)PacketType::TestPacket, size);
	//	SleepEx(1000, TRUE);
	//}
	*/