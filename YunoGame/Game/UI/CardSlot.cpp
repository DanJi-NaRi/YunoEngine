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

bool CardSlot::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Slot::Create(name, id, sizePx, vPos, rotZ, vScale);

    //m_pivot = PivotFromUIDirection(UIDirection::Center);
    //SetPivotDir(UIDirection::Center);
    SetSnapPoint(5.0f, WidgetClass::Card);
    return true;
}

bool CardSlot::Start()
{
    Slot::Start();

    return true;
}

bool CardSlot::Update(float dTime) {
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


