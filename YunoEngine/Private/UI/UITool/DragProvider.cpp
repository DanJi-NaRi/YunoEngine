#include "pch.h"
#
#include "DragProvider.h"
#include "IInput.h"
#include "UIFactory.h"
#include "UIConverter.h"
#include "YunoTransform.h"

#include "Slot.h"
#include "Widget.h"

//DragProvider::DragProvider(IInput* pInput)
//{
//    Clear();
//    m_pInput = pInput;
//    m_canDrag = false;
//    m_isDrag = false;
//}

static XMFLOAT2 ToOwnerLocalFromCanvas(const XMFLOAT2& canvasPos, Widget* ownerWidget)
{
    if (!ownerWidget) return canvasPos;

    if (Widget* parent = ownerWidget->GetParent())
    {
        const XMFLOAT3 local = WorldToLocalPos(
            XMFLOAT3(canvasPos.x, canvasPos.y, 0.0f),
            parent->GetWorldNoSizeMatrix()
        );
        return XMFLOAT2(local.x, local.y);
    }

    return canvasPos;
}

DragProvider::DragProvider()
{
    Clear();
    m_canDrag = false;
    m_isDrag = false;
}

DragProvider::~DragProvider()
{
    Clear();
}

/// <summary>
/// 드래그 공급자 생성자
/// </summary>
/// <param name="pInput">   인풋 매니저              </param>
/// <param name="pPos">     드래그 당할 위젯의 포지션  </param>
/// <param name="canDrag">  드래그 가능 여부           </param>
/// <returns></returns>
bool DragProvider::Init(XMFLOAT3* pPos, bool canDrag, UIFactory* uiFactory, Widget* ownerWidget) {
    m_pInput = YunoEngine::GetInput();
    if (!m_pInput) return false;

    m_pPos = pPos;  // 참조라서 포인터 연결

    if (!m_pPos) return false;
    m_bkPos = *m_pPos; // 비참조라 일회성

    m_canDrag = canDrag;
    m_isDrag = false;
    m_uiFactory = uiFactory;
    m_ownerWidget = ownerWidget;

    return true;
}

void DragProvider::Clear()
{
    m_pInput = nullptr;
    m_pPos = nullptr;
    m_pSnappedSlot = nullptr;
    m_uiFactory = nullptr;
    m_ownerWidget = nullptr;
}

void DragProvider::UpdateDrag(float dTime)
{   
    if (m_pSnappedSlot) {
        if (m_pSnappedSlot->GetSnapPoint()->useSnap) {
            m_bkPos = m_pSnappedSlot->GetPos();
            *m_pPos = m_pSnappedSlot->GetPos();
        }
    }

    if (!m_isDrag || !m_pInput || !m_pPos) return;
        
    // 갱신
    UpdatePos();
    //m_dragDelta = GetDeltaFromDragStart(pressedPos, m_mousePos);

    //std::cout << m_dragDelta.x << ", " << m_dragDelta.y << std::endl;

    m_pPos->x = m_mousePos.x + m_dragOffset.x;
    m_pPos->y = m_mousePos.y + m_dragOffset.y;

    //m_pPos->x = m_mousePos.x;
    //m_pPos->y = m_mousePos.y;
}

void DragProvider::UpdatePos()
{
    //m_mousePos = XMFLOAT2(m_pInput->GetMouseX(), m_pInput->GetMouseY());
    const float rawX = m_pInput->GetMouseX();
    const float rawY = m_pInput->GetMouseY();
    XMFLOAT3 worldPos(rawX, rawY, 0.0f);
    if (m_uiFactory) {
        const UICanvasMapping mapping = m_uiFactory->GetCanvasMapping();
        if (mapping.valid && mapping.canvasScale.x != 0.0f && mapping.canvasScale.y != 0.0f) {
            worldPos.x = (rawX - mapping.letterboxOffset.x) / mapping.canvasScale.x;
            worldPos.y = (rawY - mapping.letterboxOffset.y) / mapping.canvasScale.y;
        }
    }

    if (m_ownerWidget) {
        if (Widget* parent = m_ownerWidget->GetParent()) {
            worldPos = WorldToLocalPos(worldPos, parent->GetWorldNoSizeMatrix());
        }
    }

    m_mousePos = XMFLOAT2(worldPos.x, worldPos.y);
}

void DragProvider::StartDrag()
{
    if (m_canDrag && !m_isDrag) 
    {
        UpdatePos();
        //m_pressedPos = XMFLOAT2(m_pInput->GetPressedMouseX(), m_pInput->GetPressedMouseY());
        const float rawX = m_pInput->GetPressedMouseX();
        const float rawY = m_pInput->GetPressedMouseY();

        XMFLOAT3 pressedWorld(rawX, rawY, 0.0f);

        if (m_uiFactory) {
            const UICanvasMapping mapping = m_uiFactory->GetCanvasMapping();
            if (mapping.valid && mapping.canvasScale.x != 0.0f && mapping.canvasScale.y != 0.0f) 
            {
                pressedWorld.x = (rawX - mapping.letterboxOffset.x) / mapping.canvasScale.x;
                pressedWorld.y = (rawY - mapping.letterboxOffset.y) / mapping.canvasScale.y;
            }
        } 

        if (m_ownerWidget) {
            if (Widget* parent = m_ownerWidget->GetParent()) {
                pressedWorld = WorldToLocalPos(pressedWorld, parent->GetWorldNoSizeMatrix());
            }
        }

        m_pressedPos = XMFLOAT2(pressedWorld.x, pressedWorld.y);



        m_dragOffset = XMFLOAT2{ m_pPos->x - m_pressedPos.x, m_pPos->y - m_pressedPos.y };
        m_bkPos = *m_pPos; // 포지션 백업 // Init 때 이미 설정함, 스냅 때 갱신됨
        m_isDrag = true;  // 드래그 태그 ON, Update에서 커서 스냅
    }
}

void DragProvider::EndDrag()
{

    // 이미 커서 안에 있을 때 호출되는 것일 테니..
    if (m_isDrag) {
        //(SearchSnapPoint()) ? m_bkPos = : m_pPos = m_bkPos; // 검사 성공 시 스냅 위치에 따라 백업 포지션 갱신
        
        //std::cout << "endDrag!! bk!!" << std::endl;
        *m_pPos = m_bkPos;
        m_pressedPos = XMFLOAT2{};
        m_isDrag = false;
    }
}

bool DragProvider::IsNowDragging()
{
    return m_isDrag;
}

XMFLOAT2 DragProvider::GetDeltaFromDragStart(const XMFLOAT2& prevPos, const XMFLOAT2& nowPos)
{
    return XMFLOAT2(nowPos.x - prevPos.x , nowPos.y - prevPos.y);
}

void DragProvider::DetachSnap() {
    *m_pPos = m_bkPos;
    m_pSnappedSlot = nullptr;
}
