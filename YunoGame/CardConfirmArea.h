#pragma once
#include "Slot.h"
#include "C2S_BattlePackets.h"

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

    void SetCard(Card* card) { m_pCard; }
    void SetDirection(Direction direction) { m_direction = direction; }


    const Direction& GetDirection() { return m_direction; }
    Card* GetCard() { return m_pCard; }
    bool IsSetCard() { return (m_pCard); }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/draganddrop.png"); };    // 머테리얼 생성 (한 번만)

protected:
    Card* m_pCard = nullptr; // 해당 슬롯에 등록된 카드
    Direction m_direction = Direction::None; // 선택한 방향
};

