#pragma once
#include <queue>
#include "PieceHelper.h"

// _s : 시스템에게 보낼 때
// _p : 기물에게 보낼 때
struct PGridCmd
{
    CommandType cmdType;
    bool isDone = false;

    union
    {
        struct
        {
            float seconds;
        } wait_p, rollback_p, roll_p;
        struct
        {
            Direction dir;
        } rot_p;

        struct
        {
            float wx, wy, wz;
            float second;
        } mv_p;  // Move

        struct
        {
            GamePiece whichPiece;
        } die_s, over_s;    // Dead, Turn_Over

        struct
        {
            float disappearDissolveDuration;
        } dead_p, disappear_p;

        struct
        {
            GamePiece whichPiece;
            bool amIdead;
            float disappearDissolveDuration;
            bool isCollided;
        } hit_p;     // Hit

        struct 
        {
            GamePiece whichPiece;
            bool isCollided;
        } hit_s;

    };
};


// 패킷 구현이 안되어 테스트용으로 static Q를 만들어 쓰는중
// 패킷이 만들어지면 없어질 예정
// 패킷이 완성되면 게임매니저에 따로 큐를 두고 거기서 받아올 예정
class PlayGridQ
{
private:
    static std::queue<PGridCmd> pieceQue;

public:
    PlayGridQ();
    ~PlayGridQ();
    static void Insert(PGridCmd cmd);
    bool Empty();
    const PGridCmd Pop();
    void Clear();

public:
    static PGridCmd Rot_P(Direction dir);
    static PGridCmd Move_P(float wx, float wy, float wz, float second = 1, bool isDone = false);

    static PGridCmd Hit_S(GamePiece pieceType, bool isCollided = false);
    static PGridCmd Hit_P(bool isCollided = false);
    static PGridCmd MoveHit_P(GamePiece pieceType, bool amIdead, float disappearDissolveDuration = 0);

    static PGridCmd Dead_P(float disappearDissolveDuration);
    static PGridCmd Disappear_P(float disappearDissolveDuration);

    static PGridCmd Cmd_S(CommandType cmdType, GamePiece pieceType);
    static PGridCmd TimeCmd_P(CommandType cmdType, float seconds);

};
