#include "pch.h"
#include "ClientSession.h"

void ClientSession::Disconnect() {
	Session::Disconnect();
	SessionManager<ClientSession>::GetInstance().RemoveSession(_sessionID);
}
