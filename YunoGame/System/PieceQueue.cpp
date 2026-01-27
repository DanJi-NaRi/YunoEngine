#include "pch.h"

#include "PieceQueue.h"


PlayGridQ::PlayGridQ() = default;
PlayGridQ::~PlayGridQ() = default;

void PlayGridQ::Insert(PGridCmd cmd)
{
    pieceQue.push(cmd);
}

bool PlayGridQ::Empty()
{
    bool res = pieceQue.empty();
    return res;
}

const PGridCmd PlayGridQ::Pop()
{
    PGridCmd res = pieceQue.front();
    pieceQue.pop();
    return res;
}

PGridCmd PlayGridQ::Move_S(GamePiece pieceType, int cx, int cz)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.mv_s = { pieceType, cx, cz };
    return cmd;
}

PGridCmd PlayGridQ::Move_P(Direction dir, float wx, float wy, float wz, float speed, bool isDone)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.isDone = isDone;
    cmd.mv_p = { wx, wy, wz, dir, speed };
    return cmd;
}

PGridCmd PlayGridQ::Attack_S_TST(GamePiece pieceType)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Attack;
    cmd.atk_s.damage = 10.f;
    cmd.atk_s.whichPiece = pieceType;
    for (int i = 1; i < 36; i++)
    {
        cmd.atk_s.tileIDs[i-1] = i;
    }
    return cmd;
    
}

PGridCmd PlayGridQ::Cmd_S(CommandType cmdType, GamePiece pieceType)
{
    PGridCmd cmd;
    cmd.cmdType = cmdType;
    cmd.die_s.whichPiece = pieceType;
    return cmd;
}

std::queue<PGridCmd> PlayGridQ::pieceQue;
