#pragma once

enum class TileOccuType : uint8_t
{
    Collapesed,
    Unoccupied,
    Ally_Occupied,
    Enemy_Occupied,
    Both_Occupied
};

enum class TileWho : uint8_t        // PieceHelper의 PieceType과 동일 구조여야 함
{
    Ally1,
    Ally2,

    Enemy1,
    Enemy2,

    None
};

struct TileOccupy
{
    TileOccuType occuType;
    TileWho mainWho;
    TileWho subWho;
    TileOccupy() 
        : occuType(TileOccuType::Unoccupied), mainWho(TileWho::None), subWho(TileWho::None) {}
    TileOccupy(TileOccuType ot, TileWho w1, TileWho w2 = TileWho::None)
        : occuType(ot), mainWho(w1), subWho(w2) {}
};

enum class TileEvent : uint8_t
{
    None,
    Heal,
    Lazer,
    Meet
};

struct TileState
{
    TileOccupy to;
    TileEvent te;
    TileState() : to(TileOccupy{}), te(TileEvent::None) {}
};
