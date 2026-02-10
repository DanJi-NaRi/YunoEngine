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
    //virtual bool DownEvent() override;

    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Card; }

    void SetCardID(int id) { m_cardID = id; }
    const int GetCardID() { return m_cardID; }

protected:
    int m_cardID = -1;

};



