#include "pch.h"
#include "Minimap.h"

#include "Card.h"
#include "CardSlot.h"
#include "WidgetGridLine.h"
#include "MinimapTile.h"
#include "Grid.h"

#include "IInput.h"
#include "UIFactory.h"



constexpr int g_MinimapRow = 7;
constexpr int g_MinimapCol = 5;

Minimap::Minimap(UIFactory& uiFactory) : Image(uiFactory)
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
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);

    this->SetLayer(WidgetLayer::Panels);

    m_grid.SetGridXY(g_MinimapRow, g_MinimapCol, 50, 50);
    //m_grid.col = g_MinimapRow;
    //m_grid.row = g_MinimapCol;
    //m_grid.cellSize = Float2(90, 90);
    //m_grid.halfCells = Int2(std::floorf(m_grid.col / 2.f), std::floorf(m_grid.row / 2.f));

    Backup();

    return true;
}

bool Minimap::Start()
{
    Image::Start();

    return true;
}

void Minimap::CreateChild() {

    //m_pGridLine = m_uiFactory.CreateChild<WidgetGridLine>(m_name + L"_DebugWidgetGridLine", 
    //    Float2(m_grid.cellSize.x, m_grid.cellSize.y),XMFLOAT3(0, 0, 1),
    //    UIDirection::Center, this);

    GridSetup();

}


bool Minimap::Update(float dTime)
{
    Image::Update(dTime);


    return true;
}

bool Minimap::Submit(float dTime)
{
    Image::Submit(dTime);
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

void Minimap::GridSetup()
{
    float y = 0.0f; // 포지션 Y
    float pad = 0.0f;
    for (int row = 0; row < m_grid.row; row++)
    {
        float x = 0.0f; // 포지션 X
        for (int col = 0; col < m_grid.col; col++) {
            std::wstring tileName =
                m_name + L"_Tile(" + std::to_wstring(row) + L"," + std::to_wstring(col) + L")";

            m_pTiles.push_back(
                m_uiFactory.CreateChild<MinimapTile>(
                    tileName,
                    Float2(m_grid.cellSize.x, m_grid.cellSize.y),
                    XMFLOAT3(x, y, 1.0f),
                    UIDirection::Center,
                    this
                )
            );
            x += m_grid.cellSize.x + pad;
        }
        y += m_grid.cellSize.y + pad;
    }
}


int Minimap::GetID(int cx, int cz)
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
