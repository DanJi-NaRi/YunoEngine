#pragma once
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

    Turn_Over
};

enum class Direction : uint8_t
{
    Up,
    Down,
    Right,
    Left,

    UpLeft,
    UpRight,
    DownLeft,
    DownRight,

    Same
};


struct PieceInfo
{ 
    uint32_t id;        // 불변
    Team team;          // 불변
    Direction dir;      // 가변
    int health = 100; // 가변
    int cx, cz;         // 가변

    PieceInfo() : id(-1), cx(-1), cz(-1), dir(Direction::Same), team(Team::Undefined) {}
    PieceInfo(int _cx, int _cz, uint32_t _id, int _health, Direction _dir,  Team _team) 
        : id(_id), cx(_cx), cz(_cz), team(_team), health(_health)
    {
        dir = (_dir == Direction::Same) ? dir : _dir;
    }
};
