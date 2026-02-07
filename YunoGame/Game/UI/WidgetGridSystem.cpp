#include "pch.h"
#include "WidgetGridSystem.h"

#include "ObjectManager.h"
#include "UIManager.h"

#include "GridFactory.h"
#include "PlayQueue.h"

#include "Grid.h"
#include "WidgetGridBox.h"
#include "WidgetGridLine.h"

#include "UIFactory.h"
#include "Widget.h"


WidgetGridSystem::WidgetGridSystem(UIFactory uiFactory) : m_uiFactory(uiFactory)
{
    m_gridFactory = std::make_unique<GridFactory>();
    assert(m_gridFactory); // 여기서 바로 잡힘
}


WidgetGridSystem::~WidgetGridSystem()
{

}



void WidgetGridSystem::SetNG_P(Grid* NG_P)
{
    const Grid& grid = *NG_P;
    m_row = grid.m_row;
    m_column = grid.m_column;
    m_halfCellsX = std::floorf(grid.m_column / 2.f);
    m_halfCellsZ = std::floorf(grid.m_row / 2.f);
    m_cellSizeX = grid.m_cellSizeX;
    m_cellSizeZ = grid.m_cellSizeZ;
    m_invX = 1.f / grid.m_cellSizeX;
    m_invZ = 1.f / grid.m_cellSizeZ;
}


int WidgetGridSystem::GetID(int cx, int cz)
{
    int id = (cz * m_column + cx) + 1;
    return id;
}


Int2 WidgetGridSystem::GetCellByID(int tileID)
{
    int id = tileID - 1;
    int cx = id % m_column;
    int cz = id / m_column;
    return { cx, cz };
}


void WidgetGridSystem::CreateGrid(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_grids.push_back(std::make_unique<Grid>(row, column, cellSizeX, cellSizeZ));
}


//void WidgetGridSystem::CreateGridBox(float x, float y, float z)
//{
//    m_gridBox = m_uiFactory.CreateWidget<WidgetGridBox>(L"GridBox", g_defWidgetSize, XMFLOAT3(x, y, z));
//}
//
//
//void WidgetGridSystem::CreateGridLine(float x, float y, float z)
//{
//    auto pLine = m_uiFactory.CreateWidget<WidgetGridLine>(L"DebugGridLine", g_defWidgetSize, XMFLOAT3(x, y, z));
//    pLine->SetScale({ m_cellSizeX, m_cellSizeZ, 1 });
//}
