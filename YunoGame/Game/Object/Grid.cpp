#include "pch.h"
#include "Grid.h"


Grid::Grid(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_row = row;
    m_column = column;
    m_halfCellsX = std::floorf(m_column / 2.f);
    m_halfCellsZ = std::floorf(m_row / 2.f);
    m_cellSizeX = cellSizeX;
    m_cellSizeZ = cellSizeZ;
    m_invX = 1.f / cellSizeX;
    m_invZ = 1.f / cellSizeZ;
}


Int2 Grid::WorldToCell(float x, float z)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;

    int cx = (int)std::floor((x + halfx) * m_invX);
    int cz = (int)std::floor((halfz - z) * m_invZ);

    return { cx, cz };
}


Float2 Grid::CellToWorld(int cx, int cz)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;
    float wx = -halfx + cx * m_cellSizeX;
    float wz = -halfz + cz * m_cellSizeZ;

    return { wx, -wz };
}


Float2 Grid::CellToUIWorld(int cx, int cz)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;
    float wx = -halfx + cx * m_cellSizeX;
    float wz = -halfz + cz * m_cellSizeZ;

    return { wx, wz };
}

bool Grid::InBounds(int cx, int cz)
{
    return (0 <= cx && cx < m_column) && (0 <= cz && cz < m_row);
}

/////

GridXY::GridXY(int row, int column, float cellSizeX, float cellSizeY)
{
    row = row;
    column = column;
    halfCells.x = std::floorf(column / 2.f);
    halfCells.y = std::floorf(row / 2.f);
    cellSize.x = cellSizeX;
    cellSize.y = cellSizeY;
    inv.x = 1.f / cellSizeX;
    inv.y = 1.f / cellSizeY;
}

void GridXY::SetGridXY(int row, int column, float cellSizeX, float cellSizeY)
{
    this->row = row;
    this->col = column;
    halfCells.x = std::floorf(column / 2.f);
    halfCells.y = std::floorf(row / 2.f);
    cellSize.x = cellSizeX;
    cellSize.y = cellSizeY;
    inv.x = 1.f / cellSizeX;
    inv.y = 1.f / cellSizeY;
}


Int2 GridXY::WorldToCell(float x, float y)
{
    float halfx = halfCells.x * cellSize.x;
    float halfy = halfCells.y * cellSize.y;

    int cx = (int)std::floor((x + halfx) * inv.x);
    int cy = (int)std::floor((y + halfy) * inv.y);

    return { cx, cy };
}


Float2 GridXY::CellToWorld(int cx, int cy)
{
    float halfx = halfCells.x * cellSize.x;
    float halfy = halfCells.y * cellSize.y;

    float wx = -halfx + cx * cellSize.x;
    float wy = -halfy + cy * cellSize.y;

    return { wx, wy };
}


bool GridXY::InBounds(int cx, int cy)
{
    return (0 <= cx && cx < col) && (0 <= cy && cy < row);
}
