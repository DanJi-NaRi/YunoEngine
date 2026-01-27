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
            int cx, cz;
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
            float damage;
            uint8_t tileIDs[35];
        } atk_s;

        struct
        {
            GamePiece whichPiece;
        } die_s, over_s;    // Dead, Turn_Over

    };
};

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

    static PGridCmd Move_S(GamePiece pieceType, int cx, int cz);
    static PGridCmd Move_P(Direction dir, float wx, float wy, float wz, float speed = 1, bool isDone = false);

    static PGridCmd Attack_S_TST(GamePiece pieceType);

    static PGridCmd Cmd_S(CommandType cmdType, GamePiece pieceType);

};
