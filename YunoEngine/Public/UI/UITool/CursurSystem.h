#pragma once
#include <deque>
#include <memory>
#include <queue>

class Widget;
class Button;

struct SnapInfo {
    // 스냅 가능한 위젯 타입
    // 스냅 가능한 파생 위젯 or 이름

};

// 커서로 작용하는 추가 기능을 분리한 클래스.
// [ 주요 기능 ]
// 1. 현재 포커싱(드래그 & 드랍 등)중인 위젯 추적
// 2. 위젯 슬롯의 상태를 통해 스냅 가능 여부 비교

class CursurSystem
{
public:
    CursurSystem();
    ~CursurSystem();

    bool Init(std::deque<std::unique_ptr<Widget>>* pWidgets);
    void Clear();

    // 주요 기능
    void UpdateCheckSnap();
    bool FindSnapWidget(); // 포커스된 드래그 가능한 위젯이 있으면, 태그로 비교 후 스냅 가능한지 반환해주는 함수. 
    //void AddUseKeyWidget(Widget* pWidget) { m_useKeyWidgets.push(pWidget); } // 키 입력된 위젯

    // Getter & Setter
    Button* GetFocusedWidget(Button* pWidget = nullptr) {
        if (pWidget) pWidget = m_focusedWidget;
        return m_focusedWidget;
    };
    Button* GetUseKeyWidget(Button* pWidget = nullptr) {
        if (pWidget) pWidget = m_useKeyWidget;
        return m_useKeyWidget;
    };
    void SetFocusedWidget(Button* pWidget) { m_focusedWidget = pWidget; };
    void SetUseKeyWidget(Button* pWidget) { m_useKeyWidget = pWidget; };

    uint32_t GetUseKeyWidgetBindKey();
    int GetFocusedMouseButton() { return m_focusedMouseButton; }
    void SetFocusedMouseButton(int mouseButton) { m_focusedMouseButton = mouseButton; };
private:
    std::deque<std::unique_ptr<Widget>>* m_pWidgets = nullptr;

    
    Button* m_focusedWidget;            // 클릭 효과는 Button  클래스 파생부터 있으므로, Button* 타입.
    Button* m_useKeyWidget;             // 키를 입력한 위젯
    //std::queue<Widget*>m_useKeyWidgets; // 키를 입력한 위젯들
    int m_focusedMouseButton;           // 현재 누른 마우스 버튼 (LMB, RMB)
    bool m_isDrag;
};

inline RECT ExpandRect(const RECT& r, float padding) // 보정치를 통한 RECT 범위 확장 (RECT + padding)
{
    // float타입인 padding을 Long타입인 Rect에 맞추기 위해 아래 세 함수중에 보정할 것.
    // 
    // floor(n)  : 내림(−방향). 예) floor( 1.2)= 1, floor(-1.2)=-2
    // ceil(n)   : 올림(+방향). 예) ceil ( 1.2)= 2, ceil (-1.2)=-1
    // lround(n) : 가장 가까운 정수로 반올림. 예) lround(1.4)=1, lround(1.5)=2, lround(-1.5)=-2

    LONG p = (LONG)std::ceil(padding); // 패딩 보정 : ceil // 판정은 넉넉하게..

    RECT out = r;
    out.left -= p;
    out.top -= p;
    out.right += p;
    out.bottom += p;
    return out;
}


// IsIntersectWin32()와 동일 기능
inline bool IsIntersect(const RECT& a, const RECT& b)
{

    // 경계 닿으면 판정 O
    return !(a.right < b.left ||
        a.left > b.right ||
        a.bottom < b.top ||
        a.top > b.bottom);

    // 경계 닿으면 판정 X
    //return !(a.right <= b.left ||
    //    a.left >= b.right ||
    //    a.bottom <= b.top ||
    //    a.top >= b.bottom);
}
