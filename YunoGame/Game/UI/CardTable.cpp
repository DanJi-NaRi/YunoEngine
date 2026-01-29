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
    
    m_vScale.y = 5;
    m_vScale.x = 7;



    Backup();

    return true;
}

bool CardTable::CreateChild() {
        // 하위 위젯 생성

    m_SetCardSlots.push_back(m_uiFactory.CreateWidget<CardSlot>(m_name + L"_S0", XMFLOAT3(0, 0, 0)));
    this->Attach(m_SetCardSlots.back());


    //auto* slot1 = m_uiFactory.CreateWidget<CardSlot>(name + L"S1", XMFLOAT3(300, 0, 0));
    //this->Attach(slot1);
    //m_SetCardSlots.push_back(slot);
    //slot1 = nullptr;

    //auto* slot2 = m_uiFactory->CreateWidget<CardSlot>(name + L"S2", XMFLOAT3(600, 0, 0));
    //this->Attach(slot2);
    //m_SetCardSlots.push_back(slot);
    //slot2 = nullptr;

    return true;
}

bool CardTable::Update(float dTime)
{

    /*if (m_pInput->IsKeyDown(VK_OEM_4)) { m_vPos.x -= 50.0f * dTime; }
    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_vPos.x += 50.0f * dTime; }*/
    Image::Update(dTime);
    return false;
}

bool CardTable::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

