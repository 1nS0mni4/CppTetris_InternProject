#pragma once
#include "pch.h"
#include "Packet.h"
#include "Defines.h"
#include "SendBuffer.h"
#include "SessionManager.h"

class Session {
#pragma region Internal Structs
public:
	/**********************************************************
				[Structure only use for OVERLAPPED->hEvent ]
	***********************************************************/
	typedef struct {
		Session* session;
		WSABUF wsaBuf;
		char buf[65535];
	}PER_IO_DATA, * LPPER_IO_DATA;
#pragma endregion

#pragma region Functions
	/**********************************************************
						[Constructor / Destructor]
	***********************************************************/
public:
	Session(int sessionID);
	virtual ~Session();


	/**********************************************************
						[Getter / Setter]
	***********************************************************/
public:
	UINT32 GetSessionID() { return _sessionID; }
	SOCKET GetSocket() { return _socket; }


	/**********************************************************
				 [Init / Send / Recv / Disconnect]
	***********************************************************/
public:
	void Initialize(SOCKET socket, SOCKADDR_IN remoteAdr);

	template <typename T = Packet>
	int Send(T packet) {
		int size = 0;
		char* total = SendBuffer::GetInstance().write(packet, size);
		{
			lock_guard<std::mutex> guard(sendMtx);
			strcpy_s(sendInfo->buf, SENDBUF_SIZE, total);
			sendInfo->wsaBuf.len = size;

			sPending.push(sendInfo->wsaBuf);
		}

		if (sPending.size() <= 1)
			SendSegment();

		return size;
	}
	void Recv();
	
	virtual void Disconnect();
	void Clear();

	static void CALLBACK RecvCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
	static void CALLBACK SendCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

	/**********************************************************
				[Internal Use Purpose Functions]
	***********************************************************/
protected:
	void SendSegment();
	void OrganizeRecvBuf();

public:
	virtual void OnSend() = 0;
	virtual int OnRecv(char* packet, int size) = 0;
	virtual void OnConnect() = 0;
	virtual void OnDisconnect() = 0;
#pragma endregion

#pragma region Variables
	/**********************************************************
				[Internal Use Purpose Variables]
	***********************************************************/
protected:
	UINT32 _sessionID;
	SOCKADDR_IN _remoteAdr;
	SOCKET _socket;

	std::queue<WSABUF> sPending;
	std::mutex sendMtx;
	std::atomic<bool> recvAtm;
	int _read, _write;

	/**********************************************************
				[External Use Purpose Variables]
	***********************************************************/
public:
	DWORD sentBytes, recvBytes, flags;

	LPWSAOVERLAPPED sendOvlp, recvOvlp;
	LPPER_IO_DATA sendInfo, recvInfo;
#pragma endregion
};