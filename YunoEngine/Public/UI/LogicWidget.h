// LogicWidget.h
#pragma once
#include "Widget.h"

class LogicWidget : public Widget
{
public:
    using Widget::Widget;

    WidgetType GetWidgetType() override { return WidgetType::Logic; }
    WidgetClass GetWidgetClass() override { return WidgetClass::LogicWidget; }
protected:
    bool CreateMaterial() override { return true; }
    bool CreateMesh() override { return true; }
};
