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
void Minimap::SetupPanel() {
    ClearGrid();
    for (const auto& myWeapon : m_player.weapons) {
        TileData& tileData = m_pTiles[myWeapon.currentTile-1]->GetTileData();
        tileData.isPlayerTile = true;
        tileData.teamID = myWeapon.pId;
        tileData.unitID = myWeapon.weaponId;
        tileData.slotID = myWeapon.slotId;
    }

    for (const auto& enemyWeapon : m_enemy.weapons) {
        TileData& tileData = m_pTiles[enemyWeapon.currentTile-1]->GetTileData();
        tileData.isPlayerTile = true;
        tileData.teamID = enemyWeapon.pId;
        tileData.unitID = enemyWeapon.weaponId;
        tileData.slotID = enemyWeapon.slotId;
    }
}

void Minimap::UpdatePanel(const BattleResult& battleResult) {
    ClearGrid();
    for (const auto& pieceInfo : battleResult.order) {
        const auto& info = pieceInfo.data();
        TileData& tileData = m_pTiles[info->targetTileID]->GetTileData();
        tileData.isPlayerTile = true;
        tileData.teamID = info->pId;
        tileData.unitID = info->weaponId;
        tileData.slotID = info->slotId;
    }
}

void Minimap::UpdatePanel(const ObstacleResult& obstacleResult) {
    ClearGrid();
    for (const auto& pieceInfo : obstacleResult.unitState) {
        TileData& tileData = m_pTiles[pieceInfo.pId]->GetTileData();
        tileData.isPlayerTile = true;
        tileData.teamID = pieceInfo.pId;
        tileData.unitID = pieceInfo.weaponId;
        tileData.slotID = pieceInfo.slotId;
    }
}

/*
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
*/
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


void Minimap::StartDirChoce(int slotID)
{
    assert(!m_pTiles.empty());
    if (m_pTiles.empty()) return;

    SetButtonLock(false);

    for (auto& tile : m_pTiles) {
        auto& data = tile->GetTileData();
        // 버튼 금지 부분 허용
        if (!data.isPlayerTile) continue;
        if (data.slotID != slotID) continue;
    }

}

void Minimap::SetButtonLock(bool flag)
{
    assert(!m_pTiles.empty());
    if (m_pTiles.empty()) return;

    m_buttonLock = flag;
    if (m_buttonLock) {
        for (auto& tile : m_pTiles) {
            // 버튼 금지
            tile->SetUseLMB(false);
            tile->SetUseRMB(false);
        }
    }
    else {
        for (auto& tile : m_pTiles) {
            // 버튼 허용
            tile->SetUseLMB(true);
            tile->SetUseRMB(true);
        }
    }
}

void Minimap::OpenDirButton(int tileID) {
    assert(IsValidTileID(tileID));
    if (!IsValidTileID(tileID)) return;

    Int2 tileXY = GetCellByID(tileID);

    Int2 testXY = Int2(tileXY.x - 1, tileXY.y); // 왼쪽
    if (IsValidTileID(testXY)) { // 왼쪽 검사
        GetTileByID(testXY)->SetUseLMB(true);
        GetTileByID(testXY)->SetUseRMB(true);
    }
    testXY = Int2(tileXY.x + 1, tileXY.y); // 오른쪽
    if (IsValidTileID(testXY)) { // 오른쪽 검사
        GetTileByID(testXY)->SetUseLMB(true);
        GetTileByID(testXY)->SetUseRMB(true);
    }
    testXY = Int2(tileXY.x, tileXY.y - 1); // 위쪽
    if (IsValidTileID(testXY)) { // 위쪽 검사
        GetTileByID(testXY)->SetUseLMB(true);
        GetTileByID(testXY)->SetUseRMB(true);
    }
    testXY = Int2(tileXY.x, tileXY.y + 1); // 아래쪽
    if (IsValidTileID(testXY)) { // 아래쪽 검사
        GetTileByID(testXY)->SetUseLMB(true);
        GetTileByID(testXY)->SetUseRMB(true);
    }
}


/////////////////////////////

int Minimap::GetTileID(int cx, int cy) const
{
    assert(m_grid.col > 0 && m_grid.row > 0);

    if (cx < 0 || cx >= m_grid.col) return 0;
    if (cy < 0 || cy >= m_grid.row) return 0;
    int id = (cy * m_grid.col + cx) + 1;
    return id;
}

int Minimap::GetTileID(Int2 tileXY) const
{
    return GetTileID(tileXY.x, tileXY.y);
}

Int2 Minimap::GetCellByID(int tileID) const
{
    assert(m_grid.col > 0 && m_grid.row > 0);

    int id = tileID - 1;
    int cx = id % m_grid.col;
    int cz = id / m_grid.col;
    return { cx, cz };
}

MinimapTile* Minimap::GetTileByID(int tileID)
{
    if (!IsValidTileID(tileID)) return nullptr;
    return m_pTiles[(size_t)tileID - 1];
}

MinimapTile* Minimap::GetTileByID(Int2 tileXY)
{
    const int tileID = GetTileID(tileXY);
    return GetTileByID(tileID); // 기존 GetTileByID(int) 재사용
}

bool Minimap::IsValidTileID(int tileID) const
{
    if (tileID <= 0) return false;
    if (m_grid.col <= 0 || m_grid.row <= 0) return false;

    const int maxByGrid = m_grid.col * m_grid.row;
    if (tileID > maxByGrid) return false;

    const size_t idx = (size_t)tileID - 1;
    if (idx >= m_pTiles.size()) return false; // 이 형태가 더 안전

    if (m_pTiles[idx] == nullptr) return false;

    return true;
}

bool Minimap::IsValidTileID(Int2 tileXY) const
{
    if (m_grid.col <= 0 || m_grid.row <= 0) return false;

    if (tileXY.x < 0 || tileXY.x >= m_grid.col) return false;
    if (tileXY.y < 0 || tileXY.y >= m_grid.row) return false;

    const int tileID = (tileXY.y * m_grid.col + tileXY.x) + 1;
    return IsValidTileID(tileID);
}
