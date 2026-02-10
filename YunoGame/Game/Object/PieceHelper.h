#pragma once

#include "C2S_BattlePackets.h"

enum class GamePiece : uint8_t
{
    Ally1,
    Ally2,

    Enemy1,
    Enemy2,

    MAX,
    None = MAX
};
inline GamePiece& operator++(GamePiece& p) { p = static_cast<GamePiece>(static_cast<uint8_t>(p) + 1);  return p; }

enum class Team : uint8_t
{
    Undefined,
    Ally,
    Enemy
};

enum class CommandType : uint8_t
{
    Move,
    Attack,
    Hit,
    MoveHit,
    Dead,

    IncreaseAttackPhase,

    Turn_Over
};

struct PieceInfo
{ 
    uint32_t id;                    // 불변. objectmanager에서 unit* 찾는용.
    std::vector<uint32_t> subIds;   // 복합 무기(예: 차크람)의 추가 파츠
    Team team;                      // 불변
    Direction dir;                  // 가변

    int whichGrid = 0;              // 가변. 그리드가 한 개일 때는 0 고정.

    PieceInfo() : id(-1), dir(Direction::Same), team(Team::Undefined) {}
    PieceInfo(uint32_t _id, Direction _dir,  Team _team) 
        : id(_id), team(_team)
    {
        dir = (_dir == Direction::Same) ? dir : _dir;
    }
};
