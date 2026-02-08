#include "pch.h"
#include "CursurSystem.h"
#include "DragProvider.h"
#include "UIConverter.h"
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

    drag->IsNowDragging();          // 얘 뭐지? 왜있는거지? - 준혁
    
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
        SnapPoint* point = slot->GetSnapPoint();
        if (point->snapTargetClass != focusClass) continue; // 타겟 클래스가 아니면 continue

        if (!IsIntersect(m_focusedWidget->GetRect(), ExpandRect(point->snapRange, point->snapPadding))) continue; // 범위 안에 있지 않으면 continue


        if (point->IsSnapped()) { // 슬롯이 이미 스냅되어있으면
            std::cout << "Faild!! already Intersect!!" << std::endl;
            continue;
        }

        std::cout << "Intersect Success!!" << std::endl;
        
        // 드래그 프로바이더 슬롯 갱신
        Slot* beforeSlot = nullptr;
        if (beforeSlot = drag->GetSnappedSlot()) {  // DragProvider에 이미 등록된 슬롯이 있으면
            if (SnapPoint* beforeSP = beforeSlot->GetSnapPoint(); beforeSP->pSnapOwner == m_focusedWidget) {
                beforeSP->pSnapOwner = nullptr;
            }
        }

        drag->SetSnappedSlot(slot); // 드래그에 새 슬롯 등록
        point->pSnapOwner = m_focusedWidget; // 슬롯에 드래그 위젯 등록

        // 스냅 여부
        if (point->useSnap) {
            //스냅 O

            const XMFLOAT3 slotWorldPos = TransformPoint(
                XMFLOAT3(0.0f, 0.0f, 0.0f),
                slot->GetWorldNoSizeMatrix()
            );

            XMFLOAT3 snapPos = slotWorldPos;
            if (Widget* parent = m_focusedWidget->GetParent()) {
                snapPos = WorldToLocalPos(slotWorldPos, parent->GetWorldNoSizeMatrix());
            }

            drag->SetBkPos(snapPos);

            m_focusedWidget->SetPos(snapPos);
        }
        slot->Event(); // 스냅 X면 이벤트만
        
        

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

