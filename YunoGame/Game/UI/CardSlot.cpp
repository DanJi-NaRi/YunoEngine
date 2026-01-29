#include "pch.h"
#include "CardSlot.h"

CardSlot::CardSlot(UIFactory& uiFactory) : Slot(uiFactory)
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

    //m_pivot = PivotFromUIDirection(UIDirection::Center);
    //SetPivotDir(UIDirection::Center);
    SetDefaultSnapPoint(0.0f, WidgetClass::Card);
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


