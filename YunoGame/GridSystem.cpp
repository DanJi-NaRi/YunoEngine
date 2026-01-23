#include "pch.h"

#include "ObjectManager.h"
#include "GridLine.h"
#include "Tile.h"

#include "GridSystem.h"

GridSystem::GridSystem(ObjectManager* objmng)
{
    m_objectManager = objmng;
}

GridSystem::~GridSystem()
{
}

void GridSystem::Init(int row, int column, float cellSizeX, float cellSizeZ)
{
    m_gridFactory.Init(row, column);

    m_row = row;
    m_column = column;
    m_halfCellsX = std::floorf(m_column / 2.f);
    m_halfCellsZ = std::floorf(m_row / 2.f);
    m_cellSizeX = cellSizeX;
    m_cellSizeZ = cellSizeZ;
    m_invX = 1.f / cellSizeX;
    m_invZ = 1.f / cellSizeZ;
    m_cells = std::vector(m_column * m_row, CellState{});
}

void GridSystem::ClearCellState()
{
    for (auto& cell : m_cells)
    {
        cell = CellState{};
    }
}

int GridSystem::GetID(int cx, int cz)
{
    int id = cz * m_column + cx;
    return id;
}

bool GridSystem::InBounds(int cx, int cz)
{
    return (0 <= cx && cx < m_column) && (0 <= cz && cz < m_row);
}

std::pair<int, int> GridSystem::WorldToCell(float x, float z)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;

    int cx = (int)std::floor((x + halfx) * m_invX);
    int cz = (int)std::floor((halfz - z) * m_invZ);

    return { cx, cz };
}

std::pair<float, float> GridSystem::CellToWorld(int cx, int cz)
{
    float halfx = m_halfCellsX * m_cellSizeX;
    float halfz = m_halfCellsZ * m_cellSizeZ;
    float wx = -halfx + cx * m_cellSizeX;
    float wz = -halfz + cz * m_cellSizeZ;
    return { wx, -wz };
}

void GridSystem::CreateObject(float x, float y, float z)
{
    auto pLine = m_objectManager->CreateObject<GridLine>(L"DebugGridLine", XMFLOAT3(x, y + 0.01f, z));
    pLine->SetScale({ m_cellSizeX, 1, m_cellSizeZ });

    // 지금은 타일 매쉬와 메테리얼을 클래스 내부에서 수동으로 넣어주고 있지만
    // 후에 아트로부터 리소스를 받으면 CreateObject -> CreateObjectFromFile로 바뀌며 자동으로 로드됩니다.
    for (int i = 0; i < m_cells.size(); i++)
    {
        auto [wx, wz] = CellToWorld(i % m_column, i / m_column);
        auto pTile = m_objectManager->CreateObject<Tile>(L"Tile", XMFLOAT3(wx, y, wz));
        pTile->SetScale({ m_cellSizeX * 0.5f, 1, m_cellSizeZ * 0.5f });
        m_tileIDs.push_back(pTile->GetID());
    }

}

