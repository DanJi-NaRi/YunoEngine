#pragma once

class IInput;

// 드래그 기능 공급자
class DragProvider
{
public:
    DragProvider();
    ~DragProvider();
    bool Init(IInput* pInput, XMFLOAT3* pPos, bool canDrag);
    void Clear();
    void UpdateDrag(float dTime);
    void UpdatePos();
    void StartDrag();
    void EndDrag();
    bool IsNowDragging();
    bool SearchSnapPoint();
    XMFLOAT2 GetDeltaFromDragStart(const XMFLOAT2& prevPos, const XMFLOAT2& nowPos);

    const std::vector<XMFLOAT3>& GetSnapPoints() const { return m_snapPoint; }
    void SetBkPos(XMFLOAT3 bkPos) { m_bkPos = bkPos; }
    // 스냅 포인트 받는 함수 빼두기??
protected:
    bool m_canDrag;                 // 드래그 가능 여부
    bool m_isDrag;                  // 현재 드래그 여부
    IInput* m_pInput;

    // 소유자 Position
    XMFLOAT3* m_pPos;               // DragProvider 소유자 포지션
    XMFLOAT3 m_bkPos;               // 드래그 취소 시 백업 포지션

    // Input Position
    XMFLOAT2 m_mousePos;            // 커서 포지션    
    XMFLOAT2 m_pressedPos;            // 커서 포지션    

    // 보정 Position
    XMFLOAT2 m_dragOffset;          // 드래그 시 위젯 위치 보정 오프셋


    //XMFLOAT2 m_dragDelta;          // 커서 델타
    std::vector<XMFLOAT3> m_snapPoint; // 스냅 포인트
};

