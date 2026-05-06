#include "pch.h"

#include "MinimapQueue.h"


MinimapQ::MinimapQ() = default;
MinimapQ::~MinimapQ() = default;

void MinimapQ::Insert(PGridCmd cmd)
{
    minimapQue.push(cmd);
}

bool MinimapQ::Empty()
{
    bool res = minimapQue.empty();
    return res;
}

const PGridCmd MinimapQ::Pop()
{
    PGridCmd res = minimapQue.front();
    minimapQue.pop();
    return res;
}


PGridCmd MinimapQ::Move_S(GamePiece pieceType, int cx, int cz)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.mv_s = { pieceType, cx, cz };
    return cmd;
}

PGridCmd MinimapQ::Move_P(Direction dir, float wx, float wy, float wz, float speed, bool isDone)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.isDone = isDone;
    cmd.mv_p = { wx, wy, wz, dir, speed };
    return cmd;
}

PGridCmd MinimapQ::Attack_S_TST(GamePiece pieceType, int damagae)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Attack;
    cmd.atk_s.damage = damagae;
    cmd.atk_s.whichPiece = pieceType;
    for (int i = 1; i < 36; i++)
    {
        cmd.atk_s.tileIDs[i - 1] = i;
    }
    return cmd;

}

PGridCmd MinimapQ::Hit_S(GamePiece pieceType, int damage)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    cmd.hit.damage1 = damage;
    cmd.hit.whichPiece = pieceType;
    return cmd;
}

PGridCmd MinimapQ::Hit_P(int damage1, GamePiece pieceType, int damage2)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    cmd.hit.damage1 = damage1;
    cmd.hit.damage2 = damage2;
    cmd.hit.whichPiece = pieceType;
    return cmd;
}

PGridCmd MinimapQ::Cmd_S(CommandType cmdType, GamePiece pieceType)
{
    PGridCmd cmd;
    cmd.cmdType = cmdType;
    cmd.die_s.whichPiece = pieceType;
    return cmd;
}

std::queue<PGridCmd> MinimapQ::minimapQue;
