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
    Dead,

    AlarmTile,

    Turn_Over
};

struct PieceInfo
{ 
    uint32_t id;        // 불변
    Team team;          // 불변
    Direction dir;      // 가변
    int health = 100;   // 가변
    int cx, cz;         // 가변
    int whichGrid = 0;  // 가변. 그리드가 한 개일 때는 0 고정.

    PieceInfo() : id(-1), cx(-1), cz(-1), dir(Direction::Same), team(Team::Undefined) {}
    PieceInfo(int _cx, int _cz, uint32_t _id, int _health, Direction _dir,  Team _team) 
        : id(_id), cx(_cx), cz(_cz), team(_team), health(_health)
    {
        dir = (_dir == Direction::Same) ? dir : _dir;
    }
};
