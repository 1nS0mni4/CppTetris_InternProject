#include "pch.h"
#include "Room.h"

Room::Room(UINT32 roomID) : _roomID(roomID){
    
}

Room::~Room() {
    Clear();
}

void Room::Enter(Session* session) {
    if (session == nullptr)
        return;

    ClientSession* s = (ClientSession*)session;
    _sessions[s->GetSessionID()] = s;
    _scores[s->GetSessionID()] = 0;

    //TODO: 동기화 패킷 전송
}

void Room::Exit(Session* session) {
    if (_sessions[session->GetSessionID()] == 0)
        return;
    ClientSession* s = (ClientSession*)session;

    _sessions[s->GetSessionID()] = 0;
    _scores[s->GetSessionID()] = 0;

    //TODO: 동기화 패킷 전송

    if (_sessions.size() <= 0) {
        Clear();
    }
}

void Room::Update() {
    //TODO: 모든 유저에게 동기화 패킷 계속 송신
}

void Room::Start() {
    //TODO: 시작 패킷 / 서버타임 패킷 1회 송신
}

void Room::Clear() {
    _sessions.clear();
    _scores.clear();
}