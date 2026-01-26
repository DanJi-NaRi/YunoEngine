#pragma once
#include <queue>
#include "PieceHelper.h"


class PieceQ
{
private:
    static std::queue<PieceCmd> pieceQue;

public:
    PieceQ();
    ~PieceQ();
    static void Insert(PieceCmd cmd);
    bool Empty();
    const PieceCmd Pop();

public:
    static PieceCmd Move_S(PieceType pieceType, int cx, int cz);
    static PieceCmd Move_P(Direction dir, float wx, float wy, float wz);
};
