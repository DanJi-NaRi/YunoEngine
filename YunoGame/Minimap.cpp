#include "pch.h"
#include "Minimap.h"

#include "Card.h"
#include "CardSlot.h"
#include "WidgetGridLine.h"
#include "MinimapTile.h"
#include "Grid.h"

#include "BattlePackets.h"

#include "IInput.h"
#include "UIFactory.h"



constexpr int g_MinimapRow = 5;
constexpr int g_MinimapCol = 7;

Minimap::Minimap(UIFactory& uiFactory) : PhasePanel(uiFactory)
{
    Clear();
}

Minimap::~Minimap()
{
    Clear();
}

void Minimap::Clear()
{
    m_pGridLine = nullptr;

}

bool Minimap::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    PhasePanel::Create(name, id, sizePx, vPos, rotZ, vScale);

    this->SetLayer(WidgetLayer::Panels);

    m_grid.SetGridXY(g_MinimapRow, g_MinimapCol, 92, 92);           // 이거도 다름
    //m_grid.col = g_MinimapRow;
    //m_grid.row = g_MinimapCol;
    //m_grid.cellSize = Float2(90, 90);
    //m_grid.halfCells = Int2(std::floorf(m_grid.col / 2.f), std::floorf(m_grid.row / 2.f));

    Backup();

    return true;
}

bool Minimap::Start()
{
    PhasePanel::Start();

    return true;
}

void Minimap::CreateChild() {

    const float centerYOffset = -GetSize().y * 0.5f;

    //m_pGridLine = m_uiFactory.CreateChild<WidgetGridLine>(
    //    m_name + L"_DebugWidgetGridLine",
    //    Float2(m_grid.cellSize.x, m_grid.cellSize.y),
    //    XMFLOAT3(0.0f, centerYOffset, 0.0f),
    //    UIDirection::Center,
    //    this
    //);


    SetupGrid();

}


bool Minimap::Update(float dTime)
{
    PhasePanel::Update(dTime);

    return true;
}

bool Minimap::Submit(float dTime)
{
    PhasePanel::Submit(dTime);
    return false;
}

//void Minimap::CreateGridLine(float x, float y, float z)
//{
//    //if (m_gridBox == nullptr) return;
//
//    //auto pLine = m_manager->CreateObject<UnitGridLine>(L"DebugGridLine", XMFLOAT3(x, y + 0.01f, z));
//    //pLine->SetScale({ m_cellSizeX, 1, m_cellSizeZ });
//    //m_gridBox->Attach(pLine);
//}


void Minimap::SetUpPanel()
{

}

void Minimap::UpdatePanel(const BattleResult& battleResult) {

    for (const auto& pieceInfo : battleResult.order) {
        /*const auto& info = pieceInfo.data();
        TileData& tileData = m_pTiles[info->targetTileID]->GetTileData();
        tileData.isPlayerTile = true;
        tileData.teamID = info->pId;
        tileData.unitID = info->slotId;*/
    }
}

void Minimap::UpdatePanel(const ObstacleResult& obstacleResult) {
    for (const auto& pieceInfo : obstacleResult.unitState) {
        /*TileData& tileData = m_pTiles[pieceInfo.pId]->GetTileData();
        tileData.isPlayerTile = true;
        tileData.teamID = pieceInfo.pId;
        tileData.unitID = pieceInfo.slotId;*/
    }
}


//{
//    int index = data.targetTileID - 1;   // 0-based로 변환
//    int x = index % m_grid.col;
//    int y = index / m_grid.col;
//}

//void Minimap::GridSetup()
//{
//    float y = -m_size.y * 0.5f; // 포지션 Y
//    float pad = 0.0f;
//    for (int row = 0; row < m_grid.row; row++)
//    {
//        float x = 0.0f; // 포지션 X
//        for (int col = 0; col < m_grid.col; col++) {
//            std::wstring tileName =
//                m_name + L"_Tile(" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")";
//
//            m_pTiles.push_back(
//                m_uiFactory.CreateChild<MinimapTile>(
//                    tileName,
//                    Float2(m_grid.cellSize.x, m_grid.cellSize.y),
//                    XMFLOAT3(x, y, 0),
//                    UIDirection::Center,
//                    this
//                )
//            );
//            x += m_grid.cellSize.x + pad;
//        }
//        y += m_grid.cellSize.y + pad;
//    }
//}
void Minimap::SetupGrid()
{

    const float pad = 4.0f;

    const float gridWidth = (m_grid.col * m_grid.cellSize.x) + (pad * (m_grid.col - 1));
    const float gridHeight = (m_grid.row * m_grid.cellSize.y) + (pad * (m_grid.row - 1));

    const float baseX = -gridWidth * 0.5f + m_grid.cellSize.x * 0.5f;
    const float baseY = -GetSize().y * 0.5f + (-gridHeight * 0.5f + m_grid.cellSize.y * 0.5f);

    for (int row = 0; row < m_grid.row; row++)
    {
        for (int col = 0; col < m_grid.col; col++) {
            std::wstring tileName =
                m_name + L"_Tile(" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")";

            const float x = baseX + (m_grid.cellSize.x + pad) * col;
            const float y = baseY + (m_grid.cellSize.y + pad) * row;

            m_pTiles.push_back(
                m_uiFactory.CreateChild<MinimapTile>(
                    tileName,
                    Float2(m_grid.cellSize.x, m_grid.cellSize.y),
                    XMFLOAT3(x, y, 0),
                    UIDirection::Center,
                    this
                )
            );
            m_pTiles.back()->SetTileId(m_pTiles.size());
        }
    }
}

void Minimap::ClearGrid()
{
    for (const auto& tile : m_pTiles) {
        tile->ClearTileData(); // TileID 제외 초기화
    }
}

int Minimap::GetTileID(int cx, int cz)
{
    int id = (cz * m_grid.col + cx) + 1;
    return id;
}


Int2 Minimap::GetCellByID(int tileID)
{
    int id = tileID - 1;
    int cx = id % m_grid.col;
    int cz = id / m_grid.col;
    return { cx, cz };
}
