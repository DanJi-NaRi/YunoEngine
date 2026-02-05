#pragma once
#include "Slot.h"

class Card;


class CardConfirmArea : public Slot
{
public:
    CardConfirmArea(UIFactory& uiFactory);
    virtual ~CardConfirmArea();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    void CleanSetup(); // 다 비우기

    virtual WidgetType GetWidgetType() override { return WidgetType::Slot; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardConfirmArea; }

    bool Event(float dTime = 0) override; // AABB 성공 시 작동하는 이벤트

    Card* GetCard() { return m_pCard; }
    void SetCard(Card* card) { m_pCard; }

    bool IsSetCard() { return (m_pCard); }
    // [TODO] 머테리얼 할당정도??

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/draganddrop.png"); };    // 머테리얼 생성 (한 번만)

protected:
    Card* m_pCard = nullptr; // 해당 슬롯에 등록된 카드
};

