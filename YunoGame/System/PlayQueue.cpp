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


PGridCmd PlayGridQ::Move_S(GamePiece pieceType, int oldcx, int oldcz, int cx, int cz, bool isCollided, bool isEnemy, int damageMe, int damageU)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.mv_s = { pieceType, oldcx, oldcz, cx, cz, isCollided, isEnemy};
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

PGridCmd PlayGridQ::Hit_S(GamePiece pieceType)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    cmd.hit_s.whichPiece = pieceType;
    return cmd;
}

PGridCmd PlayGridQ::Hit_P()
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    return cmd;
}

PGridCmd PlayGridQ::MoveHit_P(GamePiece pieceType, bool amIdead, float disappearDissolveDuration)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::MoveHit;
    cmd.hit_p.whichPiece = pieceType;
    cmd.hit_p.amIdead = amIdead;
    cmd.hit_p.disappearDissolveDuration = disappearDissolveDuration;
    return cmd;
}

PGridCmd PlayGridQ::Dead_P(float disappearDissolveDuration)
{
    PGridCmd cmd;
    cmd.dead_p.disappearDissolveDuration = disappearDissolveDuration;
    return cmd;
}

PGridCmd PlayGridQ::Cmd_S(CommandType cmdType, GamePiece pieceType)
{
    PGridCmd cmd;
    cmd.cmdType = cmdType;
    cmd.over_s.whichPiece = pieceType;
    return cmd;
}

std::queue<PGridCmd> PlayGridQ::pieceQue;
