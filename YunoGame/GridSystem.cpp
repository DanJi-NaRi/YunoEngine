#include "pch.h"

#include "IMesh.h"

#include "GridSystem.h"

GridSystem::GridSystem(int row, int column, float cellSizeX, float cellSizeZ)
{
    Init(row, column, cellSizeX, cellSizeZ);
}

GridSystem::~GridSystem()
{
}

void GridSystem::Init(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_gridFactory.Init(row, column, cellSizeX, cellSizeZ);

    m_row = row;
    m_column = column;
    m_halfX = std::floor(m_column / 2.f);
    m_halfZ = std::floor(m_row / 2.f);
    m_cellSizeX = cellSizeX;
    m_cellSizeZ = cellSizeZ;
    m_invX = 1.f / cellSizeX;
    m_invZ = 1.f / cellSizeZ;
    m_cells.resize(m_row * m_column, CellState{});

}

void GridSystem::ClearCellState()
{
    for (auto& cell : m_cells)
    {
        cell = CellState{};
    }
}

bool GridSystem::InBounds(int cx, int cz)
{
    return (0 <= cx && cx < m_column) && (0 <= cz && cz < m_row);
}

std::pair<int, int> GridSystem::WorldToCell(float x, float z)
{
    return std::pair<int, int>({std::floor(x * m_invX), std::floor(z * m_invZ)});
}

std::pair<float, float> GridSystem::CellToWorld(int cx, int cz)
{
    return std::pair<float, float>({(cx + 0.5f) * m_cellSizeX, (cz + 0.5f) * m_cellSizeZ + 0.5f});
}
