#pragma once
#include "Widget.h"

class WidgetGridBox : public Widget
{
public:
    explicit WidgetGridBox(UIFactory& uiFactory);

    virtual ~WidgetGridBox();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale);
    bool Update(float dTime = 0);
    bool Submit(float dTime = 0);
};
