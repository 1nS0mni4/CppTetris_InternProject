#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "ClientSession.h"
#include "OvlpCallback.h"
#include "PacketQueue.h"
#include "RoomManager.h"
#include "JobQueue.h"
#include "JobTimer.h"
#include "ServerPacketHandler.h"

int main(int argc, char* argv[]) {
	SessionManager<ClientSession>::GetInstance().Init();
	ServerPacketHandler::GetInstance().Init();

	if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
		return -1;

	if (OvlpCallback::GetInstance().BindnListen("127.0.0.1", 9190, 50) == SOCKET_ERROR)
		return -1;

	std::thread fetchThread(&PacketQueue::Flush, &PacketQueue::GetInstance());
	std::thread olvpThread(&OvlpCallback::AcceptLoop<ClientSession>, &OvlpCallback::GetInstance());
	std::thread roomThread(&RoomManager::UpdateRooms, &RoomManager::GetInstance());
	std::thread processThread = SessionManager<ClientSession>::GetInstance().Start();
	
	std::string input;

	while (true) {
		Sleep(100);
		//std::cin >> input;

		//if (strcmp(input.c_str(), "quit") == 0 || strcmp(input.c_str(), "Quit") == 0)
		//	break;

		//TODO: 커맨드 기능 추가
	}

	PacketQueue::GetInstance().Close();
	OvlpCallback::GetInstance().Close();
	JobQueue::GetInstance().Close();
	JobTimer::GetInstance().Close();
	SessionManager<ClientSession>::GetInstance().Close();

	fetchThread.join();
	olvpThread.join();
	roomThread.join();
	processThread.join();

	std::cout << "Server Closed" << std::endl;

	return 0;
}