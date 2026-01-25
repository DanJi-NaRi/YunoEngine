#include "pch.h"
#include "CursurSystem.h"
#include "Widget.h"
#include "Button.h"
bool CursurSystem::Init(std::deque<std::unique_ptr<Widget>>* widgets)
{
    m_pWidgets = widgets;
    if (!m_pWidgets) return false;

    return true;
}
void CursurSystem::UpdateCheckSnap()
{
    if (!m_focusedWidget) return;

    for (auto& widget : *m_pWidgets) {
        if (widget->GetWidgetType() != WidgetType::Button) continue;
        switch (widget->GetWidgetClass()) {
        case WidgetClass::Card: break;
        default: break;
        }
    }
}
void CursurSystem::FindSnapWidget()
{
}
