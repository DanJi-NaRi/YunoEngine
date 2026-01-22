#pragma once
#include "GridFactory.h"

enum class MoveStatus : uint8_t
{
    Movable,
    Collapesed
};

enum class OccupyStatus : uint8_t
{
    Unoccupied,
    Ally_Occupied,
    Enemy_Occupied
};


struct CellState
{
    MoveStatus ms;
    OccupyStatus os;
    CellState() : ms(MoveStatus::Movable), os(OccupyStatus::Unoccupied) {}
};

class GridSystem
{
public:
    GridSystem(int row, int column, float cellSizeX, float cellSizeZ);
    ~GridSystem();
    
    void ClearCellState();

    bool InBounds(int cx, int cz);

    // 월드(x, z) -> 셀 좌표
    std::pair<int, int> WorldToCell(float x, float z);
    // 셀 좌표 -> 월드 중심(x, z)
    std::pair<float, float> CellToWorld(int cx, int cz);

    //int GetWidth();
    //int GetHeight();
    
private:
    void Init(int row, int column, float cellSizeX, float cellSizeZ);
private:
    int m_row, m_column;
    int m_halfX, m_halfZ;
    float m_cellSizeX, m_cellSizeZ;
    float m_invX, m_invZ;

    GridFactory m_gridFactory;

    std::vector<CellState> m_cells;
};

