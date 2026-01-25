#pragma once
#include "Slot.h"

class Card;


class CardSlot : public Slot
{
public:
    CardSlot();
    virtual ~CardSlot();

    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Slot; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardSlot; }

    Card* GetCard() { return m_pCard; }
    void SetCard(Card* card) { m_pCard; }
    

    bool IsSetCard() { return (m_pCard); }
    // [TODO] 머테리얼 할당정도??

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/woodbox.bmp"); };    // 머테리얼 생성 (한 번만)

protected:
    Card* m_pCard = nullptr; // 해당 슬롯에 등록된 카드
};

