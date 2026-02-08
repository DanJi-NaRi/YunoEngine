#include "pch.h"
#include "CardConfirmPanel.h"

#include "Card.h"
#include "CardSlot.h"
#include "CardConfirmArea.h"
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

bool CardConfirmPanel::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
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

    this->SetLayer(WidgetLayer::Panels);

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


    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S0", Float2(178, 264), XMFLOAT3(50, -400, 0),  UIDirection::LeftTop, this));
    
    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S1", Float2(178, 264), XMFLOAT3(350, -400, 0), UIDirection::LeftTop, this));
                                                                                                                          
    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S2", Float2(178, 264), XMFLOAT3(650, -400, 0), UIDirection::LeftTop, this));
                                                                                                                          
    m_setCardSlots.push_back(m_uiFactory.CreateChild<CardConfirmArea>(m_name + L"_S3", Float2(178, 264), XMFLOAT3(950, -400, 0), UIDirection::LeftTop, this));
    
    m_cardConfirmButton = m_uiFactory.CreateChild<CardConfirmButton>(m_name + L"_CardConfirmButton", Float2(367, 69), XMFLOAT3(0, -100, 0), UIDirection::LeftTop, this);
    
    m_cardCancelButton = m_uiFactory.CreateChild<CardCancelButton>(m_name + L"_CardCancelButton", Float2(367, 69), XMFLOAT3(0, -100, 0), UIDirection::LeftTop, this);
    for (auto* slot : m_setCardSlots) m_cardCancelButton->AddSlot(slot);
}


bool CardConfirmPanel::Update(float dTime)
{
    Image::Update(dTime);


    return true;
}

bool CardConfirmPanel::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

