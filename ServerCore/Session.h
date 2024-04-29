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
	typedef class {
	public:
		Session* session;
		WSABUF wsaBuf;
		char buf[SENDBUF_SIZE];

	public:

	}PER_IO_DATA, * LPPER_IO_DATA;
#pragma endregion

#pragma region Functions
	/**********************************************************
						[Constructor / Destructor]
	***********************************************************/
public:
	Session(UINT32 sessionID);
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


	//TODO: PacketPool에서 할당받고 여기에다 전달하면 될듯?
	int Send(Packet* packet) {
		int size = 0;
		{
			lock_guard<std::mutex> guard(pendMtx);
			sPending.emplace_back(packet);
		}

		if (sPending.size() <= 1)
			size = SendSegment();

		SleepEx(1, TRUE);
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
	int SendSegment();
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

	std::vector<shared_ptr<Packet>> sPending;
	std::vector<shared_ptr<Packet>> sending;

	std::mutex pendMtx;
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