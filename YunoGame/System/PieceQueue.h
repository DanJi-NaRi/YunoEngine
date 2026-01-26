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
};
