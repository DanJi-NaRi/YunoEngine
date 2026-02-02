#include "pch.h"
#include "CursurSystem.h"
#include "DragProvider.h"
#include "Widget.h"
#include "Button.h"
#include "Slot.h"
CursurSystem::CursurSystem()
{
    Clear();
}
CursurSystem::~CursurSystem()
{
    Clear();
}
bool CursurSystem::Init(std::deque<std::unique_ptr<Widget>>* pWidgets)
{
    m_pWidgets = pWidgets;
    if (!m_pWidgets) return false;

    return true;
}

void CursurSystem::Clear() {
    m_pWidgets = nullptr;
    m_focusedWidget = nullptr;
    m_useKeyWidget;
}

void CursurSystem::UpdateCheckSnap()
{
    // m_focusedWidget 갱신됐어야 됨
    // m_focusedWidget->GetButtonState() == ButtonState::Released 조건이 맞아야 됨

    if (!m_focusedWidget) return;

    if (m_focusedWidget->GetButtonState() == ButtonState::Released) {
        std::cout << "릴리즈 발견" << std::endl;
    }

    DragProvider* drag = m_focusedWidget->GetDragProvider();
    if (!drag) return;

    drag->IsNowDragging();
    
    if (m_focusedWidget->GetButtonState() == ButtonState::Released) {
        FindSnapWidget();
    }
    //if (!m_isDrag) return;
}

bool CursurSystem::FindSnapWidget()
{
    if (!m_pWidgets) return false;
    if (!m_focusedWidget) return false; // 포커스 위젯이 있을 때

    DragProvider* drag = m_focusedWidget->GetDragProvider();
    if (!drag) return false;                  
    if (!drag->IsNowDragging()) return false; // 드래그 중일 때

    if (m_focusedWidget->GetButtonState() != ButtonState::Released) return false;

    //std::cout << "Find Release" << std::endl;

    WidgetClass focusClass = m_focusedWidget->GetWidgetClass();

    for (auto& widget : *m_pWidgets) {
        if (widget->GetWidgetType() != WidgetType::Slot) continue; // 슬롯 아니면 continue
        Slot* slot = dynamic_cast<Slot*>(widget.get());
        if (!slot) continue;
        SnapPoint point = slot->GetSnapPoint();
        if (point.m_snapTargetClass != focusClass) continue;

        if (!IsIntersect(m_focusedWidget->GetRect(), ExpandRect(point.m_snapRange, point.m_snapPadding))) continue; // 범위 안에 있지 않으면 continue

        std::cout << "Intersect Success!!" << std::endl;

        XMFLOAT3 snapPos = { point.m_snapPos.x, point.m_snapPos.y, 0 };
        drag->SetBkPos(snapPos);
        m_focusedWidget->SetPos(snapPos);
        //m_focusedWidget->SetPosBK(snapPos); // 근본 백업은 냅둬야하지 않나??
        
        return true;
    }
    std::cout << "Intersect Faild!!" << std::endl;
    return false;
}

uint32_t CursurSystem::GetUseKeyWidgetBindKey()
{
    return m_useKeyWidget->GetBindKey();
}

