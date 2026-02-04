#include "pch.h"
#include "CardSelectionPanel.h"

#include "Card.h"
#include "CardSlot.h"

#include "IInput.h"
#include "UIFactory.h"

CardSelectionPanel::CardSelectionPanel(UIFactory& uiFactory) : Image(uiFactory)
{
    Clear();
}

CardSelectionPanel::~CardSelectionPanel()
{
    Clear();
}

void CardSelectionPanel::Clear()
{
}

bool CardSelectionPanel::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);

    if (!m_pInput || !m_pRenderer || !m_pTextures)
        return false;
    m_defaultMesh = GetDefWidgetMesh(); // 기본 quad 적용
    if (m_defaultMesh == 0)return false;
    if (!CreateMaterial())
        return false;

    m_MeshNode = std::make_unique<MeshNode>();

    auto mesh = std::make_unique<Mesh>();
    mesh->Create(m_defaultMesh, m_defaultMaterial, vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
    m_MeshNode->m_Meshs.push_back(std::move(mesh));

    {
        m_constant.baseColor = XMFLOAT4(1, 1, 1, 1);
        m_constant.roughRatio = 1.0f;
        m_constant.metalRatio = 1.0f;
        m_constant.shadowBias = 0.005f;
    }

    m_anchor = UIDirection::LeftTop;

    Backup();

    return true;
}

bool CardSelectionPanel::Start()
{
    Image::Start();

    return true;
}

void CardSelectionPanel::CreateChild() {
    // 고정 하위 위젯 생성
    this->SetLayer(WidgetLayer::Panels);

    /*m_CardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S0", Float2(100, 135), XMFLOAT3(-25, -180, 0), UIDirection::RightTop, this));

    m_CardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S1", Float2(100, 135),  XMFLOAT3(-145, -180, 0), UIDirection::RightTop, this));

    m_CardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S2", Float2(100, 135), XMFLOAT3(-265, -180, 0), UIDirection::RightTop, this));

    m_CardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S3", Float2(100, 135), XMFLOAT3(-390, -180, 0), UIDirection::RightTop, this));

    m_Cards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C0", Float2(100, 135), XMFLOAT3(-25, -180, 0), UIDirection::RightTop, this));

    m_Cards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C1", Float2(100, 135), XMFLOAT3(-145, -180, 0), UIDirection::RightTop, this));

    m_Cards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C2", Float2(100, 135), XMFLOAT3(-265, -180, 0), UIDirection::RightTop, this));

    m_Cards.push_back(m_uiFactory.CreateChild<Card>(m_name + L"_C3", Float2(100, 135), XMFLOAT3(-390, -180, 0), UIDirection::RightTop, this));*/

}


bool CardSelectionPanel::Update(float dTime)
{
    Image::Update(dTime);

    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_vPos.x += 50 * dTime; }
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_vPos.x -= 50 * dTime; }

    return true;
}

bool CardSelectionPanel::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

