#pragma once
#include "Button.h"
#include "CardData.h"       // 멤버로 들어야 해서 필요함

class Card : public Button
{
public:
    Card(UIFactory& uiFactory);
    virtual ~Card();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual bool IdleEvent() override;
    virtual bool HoveredEvent() override;
    virtual bool LMBPressedEvent() override;
    virtual bool RMBPressedEvent() override;
    virtual bool KeyPressedEvent(uint32_t key = 0) override;
    virtual bool LMBReleasedEvent() override;
    virtual bool RMBReleasedEvent() override;
    virtual bool KeyReleasedEvent(uint32_t key = 0) override;
    virtual bool HoverJoinEvent() override;
    virtual bool HoverLeftEvent() override;

    //virtual bool DownEvent() override;

    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Card; }

    void SetCardID(int id) { m_cardID = id; }
    int GetCardID() { return m_cardID; }

    void SetSlotID(int id) { m_slotID = id; }
    const int GetSlotID() { return m_slotID; }

    void SetDraggable(bool isDraggable) { m_isDraggable = isDraggable; }
    bool IsDraggable() const { return m_isDraggable; }

protected:

    int m_cardID = -1;  // 런타임 ID
    int m_slotID = -1; // 현재 기준 어느 슬롯 무기의 것인지
    bool m_isDraggable = false;
};



