#pragma once
#include <deque>
#include <memory>

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
    CursurSystem() = default;
    ~CursurSystem() = default;

    bool Init(std::deque<std::unique_ptr<Widget>>* widgets);

    // 주요 기능
    void UpdateCheckSnap();
    void FindSnapWidget(); // 포커스된 드래그 가능한 위젯이 있으면, 태그로 비교 후 스냅 가능한지 반환해주는 함수. 

    // Getter & Setter
    Button* GetFocusedWidget(Button* widget = nullptr) {
        if (widget) widget = m_focusedWidget;
        return m_focusedWidget;
    };
    void SetFocusedWidget(Button* widget) { m_focusedWidget = widget; };
    int GetFocusedMouseButton() { return m_focusedMouseButton; }
    void SetFocusedMouseButton(int mouseButton) { m_focusedMouseButton = mouseButton; };
private:
    std::deque<std::unique_ptr<Widget>>* m_pWidgets = nullptr;

    
    Button* m_focusedWidget;    // 클릭 효과는 Button  클래스 파생부터 있으므로, Button* 타입.
    int m_focusedMouseButton;   // 현재 누른 마우스 버튼 (LMB, RMB)
    bool m_isDrag;
};

inline bool IsIntersect(const RECT& a, const RECT& b)
{
    return !(a.right <= b.left ||
        a.left >= b.right ||
        a.bottom <= b.top ||
        a.top >= b.bottom);
}
