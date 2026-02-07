#include "pch.h"
#include "Minimap.h"

#include "Card.h"
#include "CardSlot.h"
#include "WidgetGridLine.h"

#include "IInput.h"
#include "UIFactory.h"

constexpr int g_MinimapRow = 7;
constexpr int g_MinimapColumn = 5;

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

    Backup();

    return true;
}

bool Minimap::Start()
{
    Image::Start();

    return true;
}

void Minimap::CreateChild() {

    // 고정 하위 위젯 생성

    m_pGridLine = m_uiFactory.CreateChild<WidgetGridLine>(m_name + L"_DebugWidgetGridLine", 
        Float2(m_pGrid->cellSize.x, m_pGrid->cellSize.y),XMFLOAT3(m_pGrid->cellSize.x, m_pGrid->cellSize.y, 1),
        UIDirection::Center, this);
    this->Attach(m_pGridLine);
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
