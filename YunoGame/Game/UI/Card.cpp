#include "pch.h"
#include "Card.h"

#include "YunoEngine.h"
#include "DragProvider.h"
#include "IInput.h"
#include "AudioQueue.h"

Card::Card(UIFactory& uiFactory) : Button(uiFactory)
{
    Clear();
}

Card::~Card()
{
    Clear();
}

void Card::Clear()
{
    m_slotID = 0;
}

bool Card::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);
    m_pDrag = std::make_unique<DragProvider>(); // 드래그 기능 사용
    if (!m_pDrag) return false;

    SetLayer(WidgetLayer::Card);

    m_pDrag->Init(&m_vPos, true, &m_uiFactory,this);
    return true;
}

bool Card::Update(float dTime) {
    m_pDrag->UpdateDrag(dTime);

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
    //std::cout << "(Card - LMB)PressedEvent" << std::endl;
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseClick));
    m_pDrag->StartDrag();
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
    //std::cout << "(Card - LMB)ReleasedEvent" << std::endl;
    m_pDrag->EndDrag();
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

bool Card::HoverJoinEvent()
{
    AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_MouseHover));
    return true;
}

bool Card::HoverLeftEvent()
{
    return false;
}

