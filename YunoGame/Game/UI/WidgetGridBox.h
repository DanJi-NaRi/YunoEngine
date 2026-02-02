#pragma once
#include "Widget.h"

class WidgetGridBox : public Widget
{
public:
    explicit WidgetGridBox(UIFactory& uiFactory);

    virtual ~WidgetGridBox();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    bool Update(float dTime = 0);
    bool Submit(float dTime = 0);
};
