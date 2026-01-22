#include "pch.h"
#include "GridFactory.h"

GridFactory::GridFactory()
{
}

GridFactory::~GridFactory()
{
}

void GridFactory::Init(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_row = row;
    m_column = column;
    m_cellSizeX = cellSizeX;
    m_cellSizeZ = cellSizeZ;
}

void GridFactory::GetGridInfo(int& row, int& column, float& cellSizeX, float& cellSizeZ)
{
    row = m_row;
    column = m_column;
    cellSizeX = m_cellSizeX;
    cellSizeZ = m_cellSizeZ;
}

int GridFactory::m_row;
int GridFactory::m_column;
float GridFactory::m_cellSizeX;
float GridFactory::m_cellSizeZ;
