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

	return 0;
}