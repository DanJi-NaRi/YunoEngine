#include "pch.h"
#include "CursurSystem.h"
#include "DragProvider.h"
#include "Widget.h"
#include "Button.h"
#include "Slot.h"
bool CursurSystem::Init(std::deque<std::unique_ptr<Widget>>* widgets)
{
    m_pWidgets = widgets;
    if (!m_pWidgets) return false;

    return true;
}

void CursurSystem::UpdateCheckSnap()
{
    if (!m_focusedWidget) return;

    DragProvider* drag = m_focusedWidget->GetDragProvider();
    if (!drag) return;

    drag->IsNowDragging();
    
    if (m_focusedWidget->GetButtonState() == ButtonState::Released) {
        FindSnapWidget();
    }
    //if (!m_isDrag) return;
}

void CursurSystem::FindSnapWidget()
{
    if (!m_pWidgets) return;
    if (!m_focusedWidget) return; // 포커스 위젯이 있을 때

    WidgetClass focusClass = m_focusedWidget->GetWidgetClass();

    for (auto& widget : *m_pWidgets) {
        if (widget->GetWidgetType() != WidgetType::Slot) continue; // 슬롯 아니면 continue
        Slot* slot = dynamic_cast<Slot*>(widget.get());
        if (!slot) continue;
        SnapPoint point = slot->GetSnapPoint();
        if (point.m_snapTargetClass != focusClass) continue;
        if (!IsIntersect(m_focusedWidget->GetRect(), point.m_snapRange)) continue; // 범위 안에 있지 않으면 continue


        XMFLOAT3 snapPos = { point.m_snapPos.x, point.m_snapPos.y, 0 };
        m_focusedWidget->SetPos(snapPos);
        m_focusedWidget->SetPosBK(snapPos);
        break;
    }
}

