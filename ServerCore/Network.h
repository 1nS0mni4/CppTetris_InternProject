#pragma once
#include "pch.h"
#include "Defines.h"


class Network {
SINGLETON(Network);

private:
	std::vector<SOCKET> sockets;
	

public:
	void Init();

	void Socket(int sktCnt = 1);
	void Bind(std::string ip, USHORT port = 0);
	void Listen(int backlog);
	void Accept();

	void Connect();

	void Send();
	void Recv();

	void Disconnect();
	void Disconnect(int);


};

