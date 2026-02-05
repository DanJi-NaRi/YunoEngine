#pragma once
#include "Button.h"

class CardConfirmArea;
class Card;

class CardCancelButton : public Button
{
public:
    CardCancelButton(UIFactory& uiFactory);
    virtual ~CardCancelButton();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;                     // 위젯 생성 동시에 바로 다음 차례에 자식 위젯을 생성해야 한다면 (자식 생성 공간)
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    // 버튼 이벤트 핸들
    virtual bool IdleEvent() override;                          // 아무것도 안할 때
    virtual bool HoveredEvent() override;                       // 커서가 위에 있을 때
    virtual bool LMBPressedEvent() override;                    // 왼클릭 눌렀을 때
    virtual bool RMBPressedEvent() override;                    // 우클릭 눌렀을 때
    virtual bool KeyPressedEvent(uint32_t key = 0) override;    // 바인딩한 키 눌렀을 때
    virtual bool LMBReleasedEvent() override;                   // 왼클릭 뗐을 때
    virtual bool RMBReleasedEvent() override;                   // 우클릭 뗐을 때
    virtual bool KeyReleasedEvent(uint32_t key = 0) override;   // 바인딩한 키 뗐을 때

    void AddSlot(CardConfirmArea* pSlot) { m_slots.push_back(pSlot); }
    void ClearSlot() { m_slots.clear(); }

    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::CardCancelButton; } // 바꾸기

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/select_card_back_mouseout.png"); };    // 머테리얼 생성 (한 번만)
protected:
    std::vector<CardConfirmArea*> m_slots;
};
