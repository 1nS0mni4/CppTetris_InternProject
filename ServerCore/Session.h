#pragma once
#include "pch.h"
#include "Packet.h"
#include "Defines.h"
#include "SendBuffer.h"

class Session {
#pragma region Internal Structs
public:
	/**********************************************************
				[Structure only use for OVERLAPPED->hEvent ]
	***********************************************************/
	typedef struct {
		Session* session;
		WSABUF wsaBuf;
		char buf[SENDBUF_SIZE];
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
	int Send(T* packet) {
		lock_guard<std::mutex> guard(sendMtx);



		//TODO: PacketQueue가 버퍼를 전달하는게 아니라 Packet*를 가지고 풀링해야됨.

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
	void OrganizeSendBuf();

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

	std::vector<Packet*> sPending;
	std::mutex sendMtx;
	std::atomic<bool> recvAtm;
	int _rcRead, _rcWrite;
	int _sdUsed, _sdWrite;

	/**********************************************************
				[External Use Purpose Variables]
	***********************************************************/
public:
	DWORD sentBytes, recvBytes, flags;

	LPWSAOVERLAPPED sendOvlp, recvOvlp;
	LPPER_IO_DATA sendInfo, recvInfo;
#pragma endregion
};