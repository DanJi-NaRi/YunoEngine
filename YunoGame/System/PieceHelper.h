#pragma once
enum class PieceType : uint8_t
{
    Ally1,
    Ally2,

    Eenemy1,
    Eenemy2,

    MAX
};
inline PieceType& operator++(PieceType& p) { p = static_cast<PieceType>(static_cast<uint8_t>(p) + 1);  return p; }


enum class CommandType : uint8_t
{
    Move,
    Attack
};


struct PieceCmd
{
    PieceType whichPiece;
    CommandType cmdType;
    union
    {
        struct
        {
            int cx, cz;
        }mv;  // Move
    };
};

struct PieceInfo
{
    int health = 100;
    int cx, cz;
    uint32_t id;

    PieceInfo() : cx(-1), cz(-1), id(-1) {}
    PieceInfo(int _cx, int _cz, uint32_t _id) : cx(_cx), cz(_cz), id(_id) {}
};
