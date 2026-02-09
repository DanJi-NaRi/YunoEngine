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
            GamePiece whichPiece;
            int oldcx, oldcz;
            int cx, cz;
            bool isCollided;
            bool isEnemy;
        } mv_s;  // Move

        struct
        {
            float wx, wy, wz;
            Direction dir;
            float speed;
        } mv_p;  // Move

        struct
        {
            GamePiece whichPiece;
        } die_s, over_s;    // Dead, Turn_Over

        struct
        {
            GamePiece whichPiece;
        } hit;     // Hit

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

public:

    static PGridCmd Move_S(GamePiece pieceType, int oldcx, int oldcz, int cx, int cz, bool isCollided = false, bool isEnemy = false, int damageMe = 0, int damageU = 0);
    static PGridCmd Move_P(Direction dir, float wx, float wy, float wz, float speed = 1, bool isDone = false);

    static PGridCmd Hit_S(GamePiece pieceType);
    static PGridCmd Hit_P(GamePiece pieceType = GamePiece::None);

    static PGridCmd Cmd_S(CommandType cmdType, GamePiece pieceType);

};
