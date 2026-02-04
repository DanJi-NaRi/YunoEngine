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

bool CardTable::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
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
    
    m_vScale.y = 5;
    m_vScale.x = 5;

    Backup();

    return true;
}

bool CardTable::Start()
{
    Image::Start();

    return true;
}

void CardTable::CreateChild() {
    // 고정 하위 위젯 생성
    this->SetLayer(WidgetLayer::Panels);
}


bool CardTable::Update(float dTime)
{
    Image::Update(dTime);

    if (m_pInput->IsKeyDown(VK_OEM_6)) { m_vPos.x += 50 * dTime; }
    if (m_pInput->IsKeyDown(VK_OEM_4)) { m_vPos.x -= 50 * dTime; }
    
    return true;
}

bool CardTable::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

