#pragma once

#define SINGLETON(x) public:\
	static x& GetInstance() {\
		static x instance;\
		return instance;\
	}\
\
protected:\
	x();\
	virtual ~x();\
	x& operator=(const x& other) = delete;\
	x(x& other) = delete;\
	x& operator=(const x&& other) = delete;\
	x(x&& other) = delete;

#define T_SINGLETON(x, T) public:\
	static x<T>& GetInstance() {\
		static x<T> instance;\
		return instance;\
	}\
\
protected:\
	x();\
	virtual ~x();\
	x<T>& operator=(const x& other) = delete;\
	x(x<T>& other) = delete;\
	x<T>& operator=(const x<T>&& other) = delete;\
	x(x<T>&& other) = delete;


#define INIT_SESSION_COUNT	50

#define SENDBUF_SIZE		1024
#define RECVBUF_SIZE		1024

#define PACKET_HEADER_SIZE		sizeof(USHORT) + sizeof(USHORT)

#define MAX_ROOM_COUNT		10
#define MAX_ROOM_CAPACITY	2

#define NAME_LEN			(USHORT)22
#define FIELD_LEN			216

#define PACKETDATA_POOL_SIZE		300
#define PACKETQUEUE_BUF_SIZE		1024