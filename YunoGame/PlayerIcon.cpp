#include "pch.h"
#include "PlayerIcon.h"

#include "HealthBar.h"
#include "StaminaBar.h"

//#include "UnitPiece.h"

#include "IInput.h"
#include "UIFactory.h"
#include "Gauge.h"

PlayerIcon::PlayerIcon(UIFactory& uiFactory) : Image(uiFactory)
{
    Clear();
}

PlayerIcon::~PlayerIcon()
{
    Clear();
}

void PlayerIcon::Clear()
{
    m_bars.clear();
}

void PlayerIcon::SetPlayer(int pNum)
{
    playerNum = pNum;
    switch (pNum)
    {
    case 1:
        for (auto& bar : m_bars)
            bar->SetFillDirect(FillDirection::RightToLeft);
        break;
    case 2:
        for (auto& bar : m_bars)
            bar->SetFillDirect(FillDirection::LeftToRight);
        break;
    default:
        break;
    }
}

bool PlayerIcon::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
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


void PlayerIcon::CreateChild() {
    m_bars.push_back(m_uiFactory.CreateChild<HealthBar>(m_name + L"_HealthBar", Float2(542, 17), XMFLOAT3(300, -24, 0), UIDirection::Center, WidgetLayer::HUD, this));
    m_bars.push_back(m_uiFactory.CreateChild<StaminaBar>(m_name + L"_StaminaBar", Float2(542, 17), XMFLOAT3(310, 0, 0), UIDirection::Center, WidgetLayer::HUD, this));
}

bool PlayerIcon::Start()
{
    Image::Start();

    return true;
}

bool PlayerIcon::Update(float dTime)
{
    Image::Update(dTime);


    return true;
}

bool PlayerIcon::Submit(float dTime)
{
    Image::Submit(dTime);
    return false;
}

