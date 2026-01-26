#pragma once

enum class TileOccupancy : uint8_t
{
    Collapesed,
    Unoccupied,
    Ally_Occupied,
    Enemy_Occupied,
    Both_Occupied
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
    TileOccupancy to;
    TileEvent te;
    TileState() : to(TileOccupancy::Unoccupied), te(TileEvent::None) {}
};
