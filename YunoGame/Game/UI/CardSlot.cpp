#include "pch.h"
#include "CardSlot.h"

CardSlot::CardSlot(UIManager* uiManager) : Slot(uiManager)
{
    Clear();
}

CardSlot::~CardSlot()
{
    Clear();
}

bool CardSlot::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Slot::Create(name, id, vPos);
    return true;
}

bool CardSlot::Update(float dTime)
{
    Slot::Update(dTime);
    return true;
}

bool CardSlot::Submit(float dTime)
{
    Slot::Submit(dTime);
    return true;
}


void CardSlot::Clear()
{
    m_pCard = nullptr;
}

void CardSlot::SetDefaultSnapPoint()
{
    m_snapPoint.m_snapPos = XMFLOAT2(m_vPos.x, m_vPos.y);
    //m_snapPoint.m_snapRange;
    //m_snapPoint.m_correction = 0;
}

