#include "pch.h"
#include "GridSystem.h"

#include "ObjectManager.h"
#include "UIManager.h"

#include "GridFactory.h"
#include "PlayQueue.h"

#include "Grid.h"
#include "GridBox.h"
#include "GridLine.h"

template class GridSystem<ObjectManager, Unit>;
template class GridSystem<UIManager, Widget>;


template<typename T, typename R>
GridSystem<T, R>::GridSystem(T* manager) : m_manager(manager)
{
    m_gridFactory = std::make_unique<GridFactory>();
    assert(m_manager != nullptr); // 여기서 바로 잡힘
}

template<typename T, typename R>
GridSystem<T, R>::~GridSystem()
{

}


template<typename T, typename R>
void GridSystem<T, R>::SetNG_P(Grid<R>* NG_P)
{
    const Grid<R>& grid = *NG_P;
    m_row = grid.m_row;
    m_column = grid.m_column;
    m_halfCellsX = std::floorf(grid.m_column / 2.f);
    m_halfCellsZ = std::floorf(grid.m_row / 2.f);
    m_cellSizeX = grid.m_cellSizeX;
    m_cellSizeZ = grid.m_cellSizeZ;
    m_invX = 1.f / grid.m_cellSizeX;
    m_invZ = 1.f / grid.m_cellSizeZ;
}

template<typename T, typename R>
int GridSystem<T, R>::GetID(int cx, int cz)
{
    int id = (cz * m_column + cx) + 1;
    return id;
}

template<typename T, typename R>
I2 GridSystem<T, R>::GetCellByID(int tileID)
{
    int id = tileID - 1;
    int cx = id % m_column;
    int cz = id / m_column;
    return { cx, cz };
}

template<typename T, typename R>
void GridSystem<T, R>::CreateGrid(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_grids.push_back(std::make_unique<Grid<R>>(row, column, cellSizeX, cellSizeZ));
}

template<typename T, typename R>
void GridSystem<T, R>::CreateGridBox(float x, float y, float z)
{
    m_gridBox = m_manager->CreateObject<GridBox<R>>(L"GridBox", XMFLOAT3(x, y, z));
}

template<typename T, typename R>
void GridSystem<T, R>::CreateGridLine(float x, float y, float z)
{
    if (m_gridBox == nullptr) return;

    auto pLine = m_manager->CreateObject<GridLine<R>>(L"DebugGridLine", XMFLOAT3(x, y + 0.01f, z));
    pLine->SetScale({ m_cellSizeX, 1, m_cellSizeZ });
    m_gridBox->Attach(pLine);
}


template<>
void GridSystem<UIManager, Widget>::CreateGridLine(float x, float y, float z)
{
    auto pLine = m_manager->CreateObject<GridLine<Widget>>(L"DebugGridLine", XMFLOAT3(x, y, z));
    pLine->SetScale({ m_cellSizeX, m_cellSizeZ, 1 });
}
