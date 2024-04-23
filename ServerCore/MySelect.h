#pragma once

#include <WinSock2.h>

class MySelect {
public:
	MySelect() {

	}

	~MySelect() {

	}

public:
	void Start();
	void Process();




private:
	WSADATA wsaData;
	fd_set reads, cpyReads;
};