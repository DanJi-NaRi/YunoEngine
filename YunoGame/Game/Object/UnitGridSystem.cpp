#include "pch.h"
#include "UnitGridSystem.h"

#include "ObjectManager.h"
#include "UIManager.h"

#include "GridFactory.h"
#include "PlayQueue.h"

#include "Grid.h"
#include "UnitGridBox.h"
#include "UnitGridLine.h"



UnitGridSystem::UnitGridSystem(ObjectManager* manager) : m_manager(manager)
{
    m_gridFactory = std::make_unique<GridFactory>();
    assert(m_manager != nullptr); // 여기서 바로 잡힘
}


UnitGridSystem::~UnitGridSystem()
{

}

int UnitGridSystem::GetID(int cx, int cz)
{
    int id = (cz * m_column + cx) + 1;
    return id;
}


Int2 UnitGridSystem::GetCellByID(int tileID)
{
    int id = tileID - 1;
    int cx = id % m_column;
    int cz = id / m_column;
    return { cx, cz };
}


void UnitGridSystem::CreateGrid(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_grids.push_back(std::make_unique<Grid>(row, column, cellSizeX, cellSizeZ));
}


void UnitGridSystem::CreateGridBox(float x, float y, float z)
{
    m_gridBox = m_manager->CreateObject<UnitGridBox>(L"GridBox", XMFLOAT3(x, y, z));
}


void UnitGridSystem::CreateGridLine(float x, float y, float z)
{
    if (m_gridBox == nullptr) return;

    auto pLine = m_manager->CreateObject<UnitGridLine>(L"DebugGridLine", XMFLOAT3(x, y + 0.01f, z));

    pLine->SetScale({ m_cellSizeX, 1.f, m_cellSizeZ });

    m_gridBox->Attach(pLine);
}
