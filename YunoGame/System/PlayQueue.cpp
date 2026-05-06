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

void PlayGridQ::Clear()
{
    while (!pieceQue.empty())
    {
        pieceQue.pop();
    }
}

PGridCmd PlayGridQ::Rot_P(Direction dir)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Rotate;
    cmd.rot_p.dir = dir;
    return cmd;
}

PGridCmd PlayGridQ::Move_P(float wx, float wy, float wz, float second, bool isDone)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Move;
    cmd.isDone = isDone;
    cmd.mv_p = { wx, wy, wz, second };
    return cmd;
}

PGridCmd PlayGridQ::Hit_S(GamePiece pieceType, bool isCollided)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    cmd.hit_s.whichPiece = pieceType;
    cmd.hit_s.isCollided = isCollided;
    return cmd;
}

PGridCmd PlayGridQ::Hit_P(bool isCollided)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Hit;
    cmd.hit_p.isCollided = isCollided;
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
    cmd.cmdType = CommandType::Dead;
    cmd.dead_p.disappearDissolveDuration = disappearDissolveDuration;
    return cmd;
}

PGridCmd PlayGridQ::Disappear_P(float disappearDissolveDuration)
{
    PGridCmd cmd;
    cmd.cmdType = CommandType::Disappear;
    cmd.disappear_p.disappearDissolveDuration = disappearDissolveDuration;
    return cmd;
}

PGridCmd PlayGridQ::Cmd_S(CommandType cmdType, GamePiece pieceType)
{
    PGridCmd cmd;
    cmd.cmdType = cmdType;
    cmd.over_s.whichPiece = pieceType;
    return cmd;
}

PGridCmd PlayGridQ::TimeCmd_P(CommandType cmdType, float seconds)
{
    PGridCmd cmd;
    cmd.cmdType = cmdType;
    cmd.wait_p.seconds = seconds;
    return cmd;
}

std::queue<PGridCmd> PlayGridQ::pieceQue;
