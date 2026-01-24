#pragma once
#include "Button.h"
#include "CardData.h"       // 멤버로 들어야 해서 필요함
#include "CardActData.h"    // 멤버로 들어야 해서 필요함
#include "DragProvider.h"

class Card : public Button
{
public:
    Card();
    virtual ~Card() = default;
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    virtual bool IdleEvent() override;
    virtual bool HoveredEvent() override;
    virtual bool LMBPressedEvent() override;
    virtual bool RMBPressedEvent() override;
    virtual bool KeyPressedEvent(uint32_t key = 0) override;
    virtual bool LMBReleasedEvent() override;
    virtual bool RMBReleasedEvent() override;
    virtual bool KeyReleasedEvent(uint32_t key = 0) override;
    //virtual bool DownEvent() override;

    PieceMask GetUnitMask() const { return m_data.m_allowedUnits; }
    CardData GetCardData() const { return m_data; }

protected:
    CardData m_data;                // 카드 데이터
    CardActData m_atkData;          // 공격 데이터
    DragProvider m_Drag;            // 드래그 기능 공급자
};



