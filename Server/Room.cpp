#include "pch.h"
#include "Room.h"

#include "ClientSession.h"

Room::Room(UINT32 roomID) : _roomID(roomID){
    
}

Room::~Room() {
    Clear();
}

void Room::Enter(Session* session) {
    if (session == nullptr)
        return;

    ClientSession* s = (ClientSession*)session;
    s->_room = this;

    _sessions[session->GetSessionID()] = session;
    _scores[session->GetSessionID()] = 0;

    ClientSession* other = (ClientSession*)GetOtherSession(session->GetSessionID());
    if (other == nullptr)
        return;

    StC_ChallengerDataPacket* challenger = new StC_ChallengerDataPacket();
    memcpy(challenger->name, s->name, NAME_LEN);

    StC_AlreadyInUserPacket* already = new StC_AlreadyInUserPacket();
    memcpy(already->name, other->name, NAME_LEN);

    session->Send(already);
    other->Send(challenger);
}

void Room::Exit(Session* session) {
    if (session == nullptr)
        return;

    if (_sessions[session->GetSessionID()] == 0)
        return;

    ClientSession* s = (ClientSession*)session;
    s->_room = nullptr;
    ClientSession* other = (ClientSession*)GetOtherSession(session->GetSessionID());

    if (other != 0) {
        StC_UserExitPacket* exit = new StC_UserExitPacket();
        other->Send(exit);
    }

    _sessions[session->GetSessionID()] = 0;
    _scores[session->GetSessionID()] = -1;

    if (_sessions.size() <= 0) {
        Clear();
        return;
    }

    //TODO: 동기화 패킷 전송
}

void Room::Update() {
    //TODO: 모든 유저에게 동기화 패킷 계속 송신
}

void Room::Start() {
    Session* session = GetOtherSession(0);
    Session* session2 = GetOtherSession(session->GetSessionID());

    StC_GameStartPacket* packet = new StC_GameStartPacket();
    StC_GameStartPacket* packet2 = new StC_GameStartPacket();
    
    session->Send(packet);
    session2->Send(packet2);
}

void Room::Clear() {
    SaveScore();
    _sessions.clear();
    _scores.clear();
}

Session* Room::GetOtherSession(UINT32 sessionID) {
    Session* session = nullptr;

    for (auto s : _sessions) {
        if (s.first == sessionID)
            continue;

        session = s.second;
    }

    return session;
}

void Room::UpdateScore(UINT32 sessionID, int score) {
    _scores[sessionID] += score;

    Session* other = GetOtherSession(sessionID);
    StC_UserScorePacket* packet = new StC_UserScorePacket();
    packet->score = score;
    other->Send(packet);
}

void Room::SaveScore() {
    //TODO: DB에 데이터 저장
}
