#include "pch.h"
#include "ClientSession.h"
#include "PacketQueue.h"


void ClientSession::Disconnect() {
	Session::Disconnect();
	ClientSessionManager::GetInstance().RemoveSession(_sessionID);
}

void ClientSession::OnSend() {

}

void ClientSession::OnRecv(Packet packet, USHORT packetID) {
	PacketQueue::GetInstance().Push(this, packet, packetID);
}

void ClientSession::OnConnect() {
	
}

void ClientSession::OnDisconnect() {

}