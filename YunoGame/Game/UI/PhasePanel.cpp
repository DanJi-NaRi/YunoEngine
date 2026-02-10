#include "pch.h"
#include "PhasePanel.h"

#include "Card.h"
#include "CardSlot.h"

#include "IInput.h"
#include "UIFactory.h"

PhasePanel::PhasePanel(UIFactory& uiFactory)
    : Image(uiFactory)
    , m_gameManager(GameManager::Get())
    , m_cardManager(m_gameManager.GetCardBasicManager())
    , m_rangeManager(m_gameManager.GetCardRangeManager())
    , m_player(m_gameManager.GetMyUIWeapons(), m_gameManager.GetMyHands())
    , m_enemy(m_gameManager.GetEnemyUIWeapons(), m_gameManager.GetEnemyHands())
    , m_pID(m_gameManager.GetSlotiIdx())
{
    Clear();
}
PhasePanel::~PhasePanel()
{
    Clear();
}

void PhasePanel::Clear()
{
}

bool PhasePanel::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);


    return true;
}

bool PhasePanel::Start()
{
    Image::Start();

    return true;
}

void PhasePanel::CreateChild() {
    // 고정 하위 위젯 생성
    this->SetLayer(WidgetLayer::Panels);
}


bool PhasePanel::Update(float dTime)
{
    Image::Update(dTime);
    return true;
}

bool PhasePanel::Submit(float dTime)
{
    Image::Submit(dTime);
    return true;
}

