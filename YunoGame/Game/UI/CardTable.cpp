#include "pch.h"
#include "CardTable.h"

#include "Card.h"
#include "CardSlot.h"

#include "IInput.h"
#include "UIFactory.h"

CardTable::CardTable(UIFactory& uiFactory) : Image(uiFactory)
{
    Clear();
}

CardTable::~CardTable()
{
    Clear();
}

void CardTable::Clear()
{
}

bool CardTable::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
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
    
    m_vScale.y = 1;
    m_vScale.x = 1;

    Backup();

    return true;
}

bool CardTable::Start()
{
    Image::Start();

    return true;
}

CardTable* CardTable::CreateChild() {
    // 고정 하위 위젯 생성

    m_SetCardSlots.push_back(m_uiFactory.CreateWidget<CardSlot>(m_name + L"_S0", XMFLOAT3(0, 0, 0)));
    this->Attach(m_SetCardSlots.back());

    m_SetCardSlots.push_back(m_uiFactory.CreateWidget<CardSlot>(m_name + L"_S1", XMFLOAT3(10, 0, 0)));
    this->Attach(m_SetCardSlots.back());

    m_SetCardSlots.push_back(m_uiFactory.CreateWidget<CardSlot>(m_name + L"_S2", XMFLOAT3(20, 0, 0)));
    this->Attach(m_SetCardSlots.back());

    m_SetCardSlots.push_back(m_uiFactory.CreateWidget<CardSlot>(m_name + L"_S3", XMFLOAT3(30, 0, 0)));
    this->Attach(m_SetCardSlots.back());

    m_SetCardSlots.push_back(m_uiFactory.CreateWidget<CardSlot>(m_name + L"_S4", XMFLOAT3(400, 0, 0)));
    this->Attach(m_SetCardSlots.back());

    m_SetCardSlots.push_back(m_uiFactory.CreateWidget<CardSlot>(m_name + L"_S5", XMFLOAT3(500, 0, 0)));
    this->Attach(m_SetCardSlots.back());
    m_SetCardSlots.back()->SetScale({ 100,100,100 });

    return this;
}


bool CardTable::UpdateTransform(float dTime)
{
    Image::UpdateTransform(dTime);

    return true;
}

bool CardTable::UpdateLogic(float dTime)
{
    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_vPos.x += 50 * dTime; }
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_vPos.x -= 50 * dTime; }
    

    return true;
}

bool CardTable::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

