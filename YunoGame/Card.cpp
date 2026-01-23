#include "pch.h"
#include "Card.h"

#include "YunoEngine.h"
#include "IInput.h"

Card::Card()
{
}

bool Card::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);
    m_Drag.Init(m_pInput, &m_vPos, true);
    return true;
}

bool Card::Update(float dTime)
{
    m_Drag.UpdateDrag(dTime);
    Button::Update(dTime);
    return true;
}

bool Card::Submit(float dTime)
{
    Button::Submit();
    return true;
}

bool Card::IdleEvent()
{
    return true;
}

bool Card::HoveredEvent()
{
    return true;
}

bool Card::LMBPressedEvent()
{
    std::cout << "(Card - LMB)PressedEvent" << std::endl;
    m_Drag.StartDrag();
    return true;
}

bool Card::RMBPressedEvent()
{
    return true;
}

bool Card::KeyPressedEvent(uint32_t key)
{
    return true;
}

bool Card::LMBReleasedEvent()
{
    std::cout << "(Card - LMB)ReleasedEvent" << std::endl;
    m_Drag.EndDrag();
    return true;
}

bool Card::RMBReleasedEvent()
{
    return true;
}

bool Card::KeyReleasedEvent(uint32_t key)
{
    return true;
}

