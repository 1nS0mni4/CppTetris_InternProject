#pragma once

#define SINGLETON(x) public:\
	static x& GetInstance() {\
		static x instance;\
		return instance;\
	}\
\
private:\
	x();\
	virtual ~x();\
	x& operator=(const x& other) = delete;\
	x(x& other) = delete;\
	x& operator=(const x&& other) = delete;\
	x(x&& other) = delete;


#define SENDBUF_SIZE		65535
#define RECVBUF_SIZE		65535