#include "pch.h"
#include "Slot.h"

Slot::Slot(UIFactory& uiFactory) : Widget(uiFactory)
{
}

Slot::~Slot()
{
}

bool Slot::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Widget::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_anchor = UIDirection::LeftTop;

    SetSnapPoint(0.0f, WidgetClass::Widget);

    Backup();

    return true;
}

bool Slot::Start()
{
    return true;
}

bool Slot::Update(float dTime)
{
    Widget::Update(dTime);
    
    return true;
}

bool Slot::Submit(float dTime)
{
    static bool test = true;
    if (m_time >= 1.0f)
    {
        test = !test;
        m_time -= 1.0f;
    }
    Widget::Submit(dTime);
    return true;
}

void Slot::SetSnapPoint(float padding, WidgetClass target)
{
    m_snapPoint.m_snapPos = { m_vPos.x, m_vPos.y };
    m_snapPoint.m_snapPadding = padding;
    m_snapPoint.m_snapRange = ExpandRect(m_rect, padding);
    m_snapPoint.m_snapTargetClass = target;
}




