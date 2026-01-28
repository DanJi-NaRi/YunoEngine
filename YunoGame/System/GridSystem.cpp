#include "pch.h"

#include "GridFactory.h"
#include "PieceQueue.h"

#include "ObjectManager.h"
#include "GridBox.h"
#include "GridLine.h"

#include "GridSystem.h"


GridSystem::GridSystem(ObjectManager* objmng) : m_objectManager(objmng)
{
    m_gridFactory = std::make_unique<GridFactory>();
    assert(objmng != nullptr); // 여기서 바로 잡힘
}

GridSystem::~GridSystem()
{
}


void GridSystem::Init(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_gridFactory->Init(row, column);

    m_row = row;
    m_column = column;
    m_halfCellsX = std::floorf(m_column / 2.f);
    m_halfCellsZ = std::floorf(m_row / 2.f);
    m_cellSizeX = cellSizeX;
    m_cellSizeZ = cellSizeZ;
    m_invX = 1.f / cellSizeX;
    m_invZ = 1.f / cellSizeZ;
}

int GridSystem::GetID(int cx, int cz)
{
    int id = (cz * m_column + cx) + 1;
    return id;
}

void GridSystem::CreateGridBox(float x, float y, float z)
{
    m_gridBox = m_objectManager->CreateObject<GridBox>(L"Tile", XMFLOAT3(x, y, z));
}

void GridSystem::CreateGridLine(float x, float y, float z)
{
    if (m_gridBox == nullptr) return;

    auto pLine = m_objectManager->CreateObject<GridLine>(L"DebugGridLine", XMFLOAT3(x, y + 0.01f, z));
    pLine->SetScale({ m_cellSizeX, 1, m_cellSizeZ });
    m_gridBox->Attach(pLine);
}

bool GridSystem::InBounds(int cx, int cz)
{
    return (0 <= cx && cx < m_column) && (0 <= cz && cz < m_row);
}

I2 GridSystem::WorldToCell(float x, float z)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;

    int cx = (int)std::floor((x + halfx) * m_invX);
    int cz = (int)std::floor((halfz - z) * m_invZ);

    return { cx, cz };
}

F2 GridSystem::CellToWorld(int cx, int cz)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;
    float wx = -halfx + cx * m_cellSizeX;
    float wz = -halfz + cz * m_cellSizeZ;
    return { wx, -wz };
}

