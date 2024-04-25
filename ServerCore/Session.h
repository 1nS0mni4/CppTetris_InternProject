#pragma once
#include "pch.h"
#include "Packet.h"
#include "OvlpCallback.h"

#define SESSION_RECVBUF_SIZE		1024

class Session {
public:
	/**********************************************************
				[Structure only use for OVERLAPPED->hEvent ]
	***********************************************************/
	typedef struct SessionInfo {
		Session* session;
		LPWSABUF wsaBuf;
	} SendInfo, RecvInfo, * lpSessionInfo, * lpSendInfo, * lpRecvInfo;


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
	void Init(SOCKET socket, SOCKADDR_IN remoteAdr);

	template <typename T = Packet>
	int Send(T packet);
	void Recv();
	
	void Disconnect();
	void Clear();

	static void CALLBACK RecvCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
	static void CALLBACK SendCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

	/**********************************************************
				[Internal Use Purpose Functions]
	***********************************************************/
protected:
	void SendSegment();
	void OrganizeRecvBuf(int offset, int size);


	/**********************************************************
				[Internal Use Purpose Variables]
	***********************************************************/
private:
	UINT32 _sessionID;
	SOCKADDR_IN _remoteAdr;
	SOCKET _socket;

	std::queue<WSABUF> sPending;
	std::mutex sendMtx;
	std::atomic<bool> recvAtm;
	char* recvBuf;
	int _read, _write;

	/**********************************************************
				[External Use Purpose Variables]
	***********************************************************/
public:
	DWORD sentBytes, recvBytes, flags;
	WSABUF wsaRecvBuf, wsaSendBuf;
	SessionInfo sendInfo, recvInfo;
	LPWSAOVERLAPPED sendOvlp, recvOvlp;
};