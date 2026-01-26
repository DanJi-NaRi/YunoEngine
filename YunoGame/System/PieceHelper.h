#pragma once
enum class PieceType : uint8_t
{
    Ally1,
    Ally2,

    Enemy1,
    Enemy2,

    MAX,
};
inline PieceType& operator++(PieceType& p) { p = static_cast<PieceType>(static_cast<uint8_t>(p) + 1);  return p; }

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
    DownRight
};

struct PieceCmd
{
    
    CommandType cmdType;
    union
    {
        struct
        {
            PieceType whichPiece;
            int cx, cz;
        } mv_s;  // Move

        struct
        {
            float wx, wy, wz;
            Direction dir;
        } mv_p;
    };
};

struct PieceInfo
{ 
    Team team;
    int health = 100;
    int cx, cz;
    uint32_t id;

    PieceInfo() : cx(-1), cz(-1), id(-1), team(Team::Undefined) {}
    PieceInfo(int _cx, int _cz, uint32_t _id, Team _team) : cx(_cx), cz(_cz), id(_id), team(_team) {}
};
