#pragma once

#pragma once
#include "RenderTypes.h"
#include "AnimationUnit.h"
#include "CursurSystem.h"
#include "Widget.h"

enum class ButtonState;

class Button;
class YunoDirectionalLight;
class UIFactory;

struct Float2;
struct Float3;
struct Float4;

class UIManager
{
private:
    size_t m_widgetCount;
    UINT m_widgetIDs;

    IInput* m_pInput;
    CursurSystem m_cursurSystem;
    std::unique_ptr<UIFactory> m_uiFactory;


    // Widgets
    std::deque<std::unique_ptr<Widget>> m_widgets;

    std::unordered_map<UINT, Widget*> m_widgetMap;
    std::unordered_map<std::wstring, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Widget>> m_pendingCreateQ;

    //Camera 투영
    bool m_isOrtho = false;
public:
    void SetOrthoFlag(bool flag) { m_isOrtho = flag; };

    // 상위 캔버스의 사이즈를 반환하는 함수.
    // 현재는 Canvas 개념이 없으므로 클라이언트 사이즈를 적용한다.
    void GetSurface(); 

public:
    explicit UIManager();
    virtual ~UIManager();

    bool Init(); //생성 초기화
    void Clear(); //삭제 초기화

    void ProcessPending(); //프레임 맨 마지막에 호출

    void Update(float dTime);
    void Submit(float dTime);       // 레이어 비적용 Submit
    void LayerSubmit(float dTime);  // 레이어 적용 Submit

    template<typename T>
    T* CreateWidget(const std::wstring& name, XMFLOAT3 pos);
    template<typename T>
    T* CreateWidget_Internal(const std::wstring& name, XMFLOAT3 pos);

    // 테스트용으로 추가
    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos);

    //씬 매니저에 있어도 될것같은 놈들
    Widget* FindWidget(UINT id); //id로 검색
    Widget* FindWidget(const std::wstring& name); //이름으로 검색


    void DestroyWidget(UINT id);
    void DestroyWidget(const std::wstring& name);

    const size_t GetWidgetCount() { return m_widgetCount; }
    const std::unordered_map<UINT, Widget*>& GetWidgetlist() { return m_widgetMap; }

    CursurSystem* GetCursurStstem() { return &m_cursurSystem; }

    void UpdateButtonStates(); // 모든 위젯 업데이트

    // 입력값 업데이트
    bool ProcessButtonMouse(ButtonState state, uint32_t mouseButton = 0); // 커서 <-> 위젯 Rect 위치 비교
    bool ProcessButtonKey(ButtonState state, uint32_t key);

    Float2 GetCanvasSize();

    std::vector<WidgetDesc> BuildWidgetDesc();
    void ApplyWidgetFromDesc(const std::vector<WidgetDesc>& wds);
private:
    void CheckDedicateWidgetName(std::wstring& name);


    // 프레임 상수버퍼 관리
private:
    Frame_Data_Dir dirData;
    void FrameDataUpdate();
    void FrameDataSubmit();
};


template<typename T>
T* UIManager::CreateWidget(const std::wstring& name, XMFLOAT3 pos)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must Derived Widget(UIObject, UIManager.h)");

    std::wstring newname = name;

    auto widget = std::make_unique<T>(*m_uiFactory);
    CheckDedicateWidgetName(newname);

    widget->Create(newname, m_widgetIDs, pos); // 생성 (유니티로 치면 Awake)

    auto* pWidget = widget.get();

    m_pendingCreateQ.emplace_back(std::move(widget));
    m_widgetIDs++;

    return pWidget;
}


// 테스트용
template<typename T>
T* UIManager::CreateObject(const std::wstring& name, XMFLOAT3 pos)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must Derived Widget(UIObject, UIManager.h)");

    std::wstring newname = name;

    auto widget = std::make_unique<T>(*m_uiFactory);
    CheckDedicateWidgetName(newname);

    widget->Create(name, m_widgetIDs, pos);

    auto* pWidget = widget.get();

    m_pendingCreateQ.emplace_back(std::move(widget));
    m_widgetIDs++;

    return pWidget;
}
