#include "pch.h"
#include "Session.h"
#include "Packet.h"
#include "OvlpCallback.h"

Session::Session(int sessionID) 
	: _sessionID(sessionID) {
	
}

Session::~Session() {

}

void Session::Init(SOCKET socket, SOCKADDR_IN remoteAdr) {
	_socket = socket;
	_remoteAdr = remoteAdr;

	sentBytes = recvBytes = flags = 0;

	//TODO: Register SessionManager and get ID;
	//TODO: Receive Memory buffer from SendBuffer, RecvBuffer;
}

int Session::Send(Packet packet) {

	

	return 0;
}

int Session::Recv() {
	return 0;
	
}