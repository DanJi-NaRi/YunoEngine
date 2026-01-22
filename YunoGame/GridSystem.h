#pragma once

enum class MoveStatus : uint8_t
{
    Movable,
    Collapesed
};

enum class OccupyStatus : uint8_t
{
    Unoccupied,
    Occupied
};

enum class OccupyType : uint8_t
{
    Ally,
    Enemy
};

struct CellState
{
    MoveStatus ms;
    OccupyStatus os;
    OccupyType ot;
};

class GridSystem
{
private:
    int m_w, m_h;
    float m_cellSize, m_inv;
    std::vector<CellState> m_cells;
};

