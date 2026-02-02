#include "pch.h"

#include "PlayQueue.h"


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


PGridCmd PlayGridQ::Move_S(GamePiece pieceType, int cx, int cz, bool isCollided, bool isEnemy, int damageMe, int damageU)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.mv_s = { pieceType, cx, cz, isCollided, isEnemy, damageMe, damageU };
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

PGridCmd PlayGridQ::Attack_S_TST(GamePiece pieceType, int damagae)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Attack;
    cmd.atk_s.damage = damagae;
    cmd.atk_s.whichPiece = pieceType;
    for (int i = 1; i < 36; i++)
    {
        cmd.atk_s.tileIDs[i-1] = i;
    }
    return cmd;
    
}

PGridCmd PlayGridQ::Hit_S(GamePiece pieceType, int damage)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    cmd.hit.damage1 = damage;
    cmd.hit.whichPiece = pieceType;
    return cmd;
}

PGridCmd PlayGridQ::Hit_P(int damage1, GamePiece pieceType, int damage2)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    cmd.hit.damage1 = damage1;
    cmd.hit.damage2 = damage2;
    cmd.hit.whichPiece = pieceType;
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
