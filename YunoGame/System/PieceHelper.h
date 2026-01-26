#pragma once
enum class GamePiece : uint8_t
{
    Ally1,
    Ally2,

    Enemy1,
    Enemy2,

    MAX,
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
    Attack
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

struct PieceCmd
{
    
    CommandType cmdType;
    bool isEnd = false;
    union
    {
        struct
        {
            GamePiece whichPiece;
            int cx, cz;
        } mv_s;  // Move

        struct
        {
            float wx, wy, wz;
            Direction dir;
            float speed;
        } mv_p;
    };
};

struct PieceInfo
{ 
    Team team;          // 불변
    Direction dir;      // 가변
    int health = 100;   // 가변
    int cx, cz;         // 가변
    uint32_t id;        // 불변

    PieceInfo() : cx(-1), cz(-1), id(-1), dir(Direction::Same), team(Team::Undefined) {}
    PieceInfo(int _cx, int _cz, uint32_t _id, Direction _dir,  Team _team) 
        : cx(_cx), cz(_cz), id(_id), team(_team) 
    {
        dir = (_dir == Direction::Same) ? dir : _dir;
    }
};
