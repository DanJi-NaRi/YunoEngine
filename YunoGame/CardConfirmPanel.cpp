#include "pch.h"
#include "CardConfirmPanel.h"

#include "Card.h"
#include "CardSlot.h"
#include "CardConfirmButton.h"
#include "CardCancelButton.h"

#include "IInput.h"
#include "UIFactory.h"

CardConfirmPanel::CardConfirmPanel(UIFactory& uiFactory) : Image(uiFactory)
{
    Clear();
}

CardConfirmPanel::~CardConfirmPanel()
{
    Clear();
}

void CardConfirmPanel::Clear()
{
}

bool CardConfirmPanel::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Image::Create(name, id, vPos);

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

    m_vScale.y = 5;
    m_vScale.x = 5;

    Backup();

    return true;
}

bool CardConfirmPanel::Start()
{
    Image::Start();

    return true;
}

void CardConfirmPanel::CreateChild() {
    // 고정 하위 위젯 생성
    this->SetLayer(WidgetLayer::Panels);

    m_SetCardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S0", XMFLOAT3(25, -240, 0), Float2(100, 135), UIDirection::LeftTop, this));

    m_SetCardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S1", XMFLOAT3(145, -240, 0), Float2(100, 135), UIDirection::LeftTop, this));

    m_SetCardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S2", XMFLOAT3(265, -240, 0), Float2(100, 135), UIDirection::LeftTop, this));
    
    m_SetCardSlots.push_back(m_uiFactory.CreateChild<CardSlot>(m_name + L"_S3", XMFLOAT3(390, -240, 0), Float2(100, 135), UIDirection::LeftTop, this));

    m_CardConfirmButton = m_uiFactory.CreateChild<CardConfirmButton>(m_name + L"_CardConfirmButton", XMFLOAT3(100, -100, 0), Float2(200, 50), UIDirection::LeftTop, this);

    //m_CardCancelButton  = m_uiFactory.CreateChild<CardCancelButton>(m_name + L"_CardCancelButton", XMFLOAT3(145, -100, 0), Float2(400, 50), UIDirection::LeftTop, this);
}


bool CardConfirmPanel::Update(float dTime)
{
    Image::Update(dTime);

    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_vPos.x += 50 * dTime; }
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_vPos.x -= 50 * dTime; }

    return true;
}

bool CardConfirmPanel::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

