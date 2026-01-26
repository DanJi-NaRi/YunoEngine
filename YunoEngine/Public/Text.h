#pragma once
#include "Widget.h"

class Text : public Widget {
public:
    Text();

    virtual WidgetType GetWidgetType() override { return WidgetType::Text; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Text; }
};
