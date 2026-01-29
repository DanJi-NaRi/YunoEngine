#pragma once
#include "Slot.h"

class Card;


class CardSlot : public Slot
{
public:
    CardSlot(UIFactory& uiFactory);
    virtual ~CardSlot();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Slot; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardSlot; }

    Card* GetCard() { return m_pCard; }
    void SetCard(Card* card) { m_pCard; }
    

    bool IsSetCard() { return (m_pCard); }
    // [TODO] 머테리얼 할당정도??

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/black.png"); };    // 머테리얼 생성 (한 번만)

protected:
    SnapPoint m_snapPoint;
    Card* m_pCard = nullptr; // 해당 슬롯에 등록된 카드
};

