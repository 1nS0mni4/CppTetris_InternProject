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


int main(int argc, char* argv[]) {
	SessionManager<ServerSession>::GetInstance().Init();
	std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

	if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
		return -1;

	ServerSession* session = OvlpCallback::GetInstance().Connect<ServerSession>(PF_INET, "127.0.0.1", 9190);
	if (session == nullptr)
		return -1;

	while (true) {

	}

	OvlpCallback::GetInstance().Close();
	PacketQueue::GetInstance().Close();

	packetFetchLoop.join();

	std::cout << "Server Closed" << std::endl;

	return 0;
}
void ErrorHandling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(-1);
}