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

    //TODO: ����ȭ ��Ŷ ����
}

void Room::Exit(Session* session) {
    if (_sessions[session->GetSessionID()] == 0)
        return;
    ClientSession* s = (ClientSession*)session;

    _sessions[s->GetSessionID()] = 0;
    _scores[s->GetSessionID()] = 0;

    //TODO: ����ȭ ��Ŷ ����

    if (_sessions.size() <= 0) {
        Clear();
    }
}

void Room::Update() {
    //TODO: ��� �������� ����ȭ ��Ŷ ��� �۽�
}

void Room::Start() {
    //TODO: ���� ��Ŷ / ����Ÿ�� ��Ŷ 1ȸ �۽�
}

void Room::Clear() {
    _sessions.clear();
    _scores.clear();
}