#include "pch.h"

#include "PieceQueue.h"


PieceQ::PieceQ() = default;
PieceQ::~PieceQ() = default;

void PieceQ::Insert(PieceCmd cmd)
{
    pieceQue.push(cmd);
}

bool PieceQ::Empty()
{
    bool res = pieceQue.empty();
    return res;
}

const PieceCmd PieceQ::Pop()
{
    PieceCmd res = pieceQue.front();
    pieceQue.pop();
    return res;
}

std::queue<PieceCmd> PieceQ::pieceQue;
