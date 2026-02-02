#include "pch.h"
#include "WidgetGridBox.h"

WidgetGridBox::WidgetGridBox(UIFactory& uiFactory) : Widget(uiFactory)
{
    m_name = L"WidgetGridBox";
}

WidgetGridBox::~WidgetGridBox()
{
}

bool WidgetGridBox::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Widget::Create(name, id, vPos);
    Backup();

    return true;
}

bool WidgetGridBox::Update(float dTime)
{
    Widget::Update(dTime);
    return true;
}

bool WidgetGridBox::Submit(float dTime)
{
    Widget::Submit(dTime);
    return true;
}
