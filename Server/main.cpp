#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "pch.h"
#include "OvlpCallback.h"

int main(int argc, char* argv[]) {
	OvlpCallback& netMod = OvlpCallback::GetInstance();

	if (netMod.Start() == SOCKET_ERROR)
		return -1;

	if (netMod.BindnListen("127.0.0.1", 9190) == -1)
		return -1;

	std::thread t1(&OvlpCallback::AcceptLoop, &netMod);

	std::string input;

	while (true) {
		std::cin >> input;

		if (strcmp(input.c_str(), "quit") == 0 || strcmp(input.c_str(), "Quit") == 0)
			break;

		//TODO: console commands
	}
	netMod.Close();
	t1.join();

	std::cout << "Server Closed" << std::endl;

	return 0;
}