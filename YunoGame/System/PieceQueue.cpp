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

PieceCmd PieceQ::Move_S(GamePiece pieceType, int cx, int cz, bool isEnd)
{
    PieceCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.isEnd = isEnd;
    cmd.mv_s = { pieceType, cx, cz };
    return cmd;
}

PieceCmd PieceQ::Move_P(Direction dir, float wx, float wy, float wz, float speed, bool isEnd)
{
    PieceCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.isEnd = isEnd;
    cmd.mv_p = { wx, wy, wz, dir, speed };
    return cmd;
}

std::queue<PieceCmd> PieceQ::pieceQue;
