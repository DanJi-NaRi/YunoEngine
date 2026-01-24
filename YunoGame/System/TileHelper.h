#pragma once

enum class TileOccupancy : uint8_t
{
    Collapesed,
    Unoccupied,
    Ally_Occupied,
    Enemy_Occupied
};

enum class TileEvent : uint8_t
{
    None,
    Heal,
    Lazer,
    Fall
};

struct TileState
{
    TileOccupancy co;
    TileEvent ce;
    TileState() : co(TileOccupancy::Unoccupied), ce(TileEvent::None) {}
};
