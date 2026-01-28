#include "pch.h"
#
#include "DragProvider.h"
#include "IInput.h"
#include "YunoTransform.h"

//DragProvider::DragProvider(IInput* pInput)
//{
//    Clear();
//    m_pInput = pInput;
//    m_canDrag = false;
//    m_isDrag = false;
//}

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
bool DragProvider::Init(IInput* pInput, XMFLOAT3* pPos, bool canDrag) {
    m_pInput = pInput;
    if (!m_pInput) return false;

    m_pPos = pPos;  // 참조라서 포인터 연결

    if (!m_pPos) return false;
    m_bkPos = *m_pPos; // 비참조라 일회성

    m_canDrag = canDrag;
    m_isDrag = false;

    return true;
}

void DragProvider::Clear()
{
    m_pInput = nullptr;
    m_pPos = nullptr;
}

void DragProvider::UpdateDrag(float dTime)
{   
    if (!m_isDrag || !m_pInput || !m_pPos) return;
        
    // 갱신
    UpdatePos();
    //m_dragDelta = GetDeltaFromDragStart(pressedPos, m_mousePos);

    //std::cout << m_dragDelta.x << ", " << m_dragDelta.y << std::endl;

    //if (m_dragDelta.x != 0 || m_dragDelta.y != 0) {
    //    // [TODO] 마우스에 붙을 때 위젯 가운데로 가게끔 보정
    //    m_pPos->x = m_bkPos.x + m_dragDelta.x;
    //    m_pPos->y = m_bkPos.y + m_dragDelta.y;
    //}

    m_pPos->x = m_mousePos.x + m_dragOffset.x;
    m_pPos->y = m_mousePos.y + m_dragOffset.y;

    //m_pPos->x = m_mousePos.x;
    //m_pPos->y = m_mousePos.y;
}

void DragProvider::UpdatePos()
{
    m_mousePos = XMFLOAT2(m_pInput->GetMouseX(), m_pInput->GetMouseY());
}

void DragProvider::StartDrag()
{
    if (m_canDrag && !m_isDrag) {
        UpdatePos();
        m_pressedPos = XMFLOAT2(m_pInput->GetPressedMouseX(), m_pInput->GetPressedMouseY());
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
        m_isDrag = false; // [TODO] 스냅 태그 확인하고 원위치 체크해야됨
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
