#include "pch.h"
#include "CardTable.h"

#include "Card.h"
#include "CardSlot.h"

#include "UIManager.h"

CardTable::CardTable()
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
    Widget::Create(name, id, vPos);

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

    m_anchor = Anchor::LeftTop;
    
    m_vScale.y = -5;
    m_vScale.x = 7;

    //m_SetCardSlots.push_back(UIManager::CreateWidget<CardTable>(L"tstCardTable", XMFLOAT3(500, 500, 0)));



    Backup();

    return true;
    return false;
}

bool CardTable::Update(float dTime)
{
    Widget::Update(dTime);
    return false;
}

bool CardTable::Submit(float dTime)
{
    Widget::Submit(dTime);
    return false;
}

