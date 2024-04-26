#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "ServerSession.h"
#include "OvlpCallback.h"
#include "PacketQueue.h"

void ErrorHandling(const char* message);

static ServerSession* session = nullptr;

int main(int argc, char* argv[]) {
	SessionManager<ServerSession>::GetInstance().Init();
	std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

	if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
		return -1;

	if((session = OvlpCallback::GetInstance().Connect<ServerSession>(AF_INET, "127.0.0.1", 9190)) == nullptr)
	   return -1;

	std::string input;

	while (true) {
		SleepEx(100, TRUE);

		TestPacket packet;
		packet.data = 10;
		session->Send<TestPacket>(packet);
	}

	OvlpCallback::GetInstance().Close();
	PacketQueue::GetInstance().Close();

	packetFetchLoop.join();

	std::cout << "Server Closed" << std::endl;

	return 0;
}


	/*WSADATA wsaData;
SOCKET hSocket;
SOCKADDR_IN sendAdr;

WSABUF dataBuf;
char msg[] = "network is computer";
DWORD sendBytes = 0;

WSAEVENT evObj;
WSAOVERLAPPED overlapped;

if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	ErrorHandling("WSAStartup() error");

hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
memset(&sendAdr, 0, sizeof(sendAdr));
sendAdr.sin_family = AF_INET;
sendAdr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
sendAdr.sin_port = htons(9190);

if (connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
	ErrorHandling("connect() error");

evObj = WSACreateEvent();
memset(&overlapped, 0, sizeof(overlapped));
overlapped.hEvent = evObj;
dataBuf.len = strlen(msg) + 1;
dataBuf.buf = msg;

while (true) {
	if (WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() == WSA_IO_PENDING) {
			puts("Background data send");
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
		}
		else {
			ErrorHandling("WSASend() error");
		}
	}

	printf("Send data size: %d\n", sendBytes);
	SleepEx(1000, FALSE);
}

WSACloseEvent(evObj);
closesocket(hSocket);
WSACleanup();

return 0;

*/


void ErrorHandling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(-1);
}