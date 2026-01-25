#include "pch.h"
#include "CardSlot.h"

CardSlot::CardSlot()
{
    Clear();
}

CardSlot::~CardSlot()
{
    Clear();
}


void CardSlot::Clear()
{
    m_pCard = nullptr;
}

