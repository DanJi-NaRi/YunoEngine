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


I2 Grid::WorldToCell(float x, float z)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;

    int cx = (int)std::floor((x + halfx) * m_invX);
    int cz = (int)std::floor((halfz - z) * m_invZ);

    return { cx, cz };
}


F2 Grid::CellToWorld(int cx, int cz)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;
    float wx = -halfx + cx * m_cellSizeX;
    float wz = -halfz + cz * m_cellSizeZ;

    return { wx, -wz };
}


F2 Grid::CellToUIWorld(int cx, int cz)
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
