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
            int damage;
            uint8_t tileIDs[35];
        } atk_s;            // Attack

        struct
        {
            GamePiece whichPiece;
        } die_s, over_s;    // Dead, Turn_Over

        struct
        {
            GamePiece whichPiece;
            int damage1;
            int damage2;
        } hit;     // Hit

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

    static PGridCmd Attack_S_TST(GamePiece pieceType, int damagae = 10);
    static PGridCmd Hit_S(GamePiece pieceType, int damagae);
    static PGridCmd Hit_P(int damagae1, GamePiece pieceType = GamePiece::None, int damage2 = 0);

    static PGridCmd Cmd_S(CommandType cmdType, GamePiece pieceType);

};
