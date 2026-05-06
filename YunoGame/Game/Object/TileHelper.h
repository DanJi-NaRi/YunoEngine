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
    TileWho who;
    TileOccupy() 
        : occuType(TileOccuType::Unoccupied), who(TileWho::None) {}
    TileOccupy(TileOccuType ot, TileWho w)
        : occuType(ot), who(w) {}
};

enum class TileEvent : uint8_t
{
    None,
    Heal,
    Lazer
};

struct TileState
{
    TileOccupy to;
    TileEvent te;
    std::vector<uint32_t> effectIDs;
    TileState() : to(TileOccupy{}), te(TileEvent::None) {}
};
