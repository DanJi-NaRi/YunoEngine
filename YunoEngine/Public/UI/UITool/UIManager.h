#pragma once

#pragma once
#include "RenderTypes.h"

#include "AnimationUnit.h"
#include "CursurSystem.h"
#include "Widget.h"

enum class ButtonState;

class Button;
class YunoDirectionalLight;

class UIManager
{
private:
    size_t m_widgetCount;
    UINT m_widgetIDs;

    IInput* m_pInput;
    CursurSystem m_cursurSystem;

    // Widgets
    std::deque<std::unique_ptr<Widget>> m_widgets;

    std::unordered_map<UINT, Widget*> m_widgetMap;
    std::unordered_map<std::wstring, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Widget>> m_pendingCreateQ;

    //Camera 투영
    bool m_isOrtho = false;

    template<typename T>
    T* CreateWidget(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //재귀 위젯 생성용

    std::unique_ptr<MeshNode>CreateMeshNode(const std::wstring& filepath);

    std::unique_ptr<YunoDirectionalLight> m_directionLight; // 필요할까?

public:
    void CreateDirLight();
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
    void Submit(float dTime);

    template<typename T>
    T* CreateWidget(const std::wstring& name, XMFLOAT3 pos);

    //씬 매니저에 있어도 될것같은 놈들
    const Widget* FindWidget(UINT id); //id로 검색
    const Widget* FindWidget(const std::wstring& name); //이름으로 검색


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
private:
    void CheckDedicateWidgetName(std::wstring& name);


    // 프레임 상수버퍼 관리
private:
    Frame_Data_Dir dirData;
    void FrameDataUpdate();
    void FrameDataSubmit();
};


// 김장후 - 
// 현재 Object와 같은 ID 체계를 사용하고 있음. 분리할지 고민중..
// 매니저 분리까지는 굳이 필요 없을 것 같아서 일단은 큐만 추가해서 사용 중.
template<typename T>
T* UIManager::CreateWidget(const std::wstring& name, XMFLOAT3 pos)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must Derived Widget(UIObject, UIManager.h)");

    std::wstring newname = name;

    auto widget = std::make_unique<T>(this);
    CheckDedicateWidgetName(newname);

    widget->Create(name, m_widgetIDs, pos);

    auto* pWidget = widget.get();

    m_pendingCreateQ.emplace_back(std::move(widget));
    m_widgetIDs++;

    return pWidget;
}

// 계층구조 오브젝트 재귀 생성용 (현재 안씀)
template<typename T>
T* UIManager::CreateWidget(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must Derived Widget(UIObject, UIManager.h)");

    std::wstring newname = name + L'_' + node->m_name;

    auto widget = std::make_unique<T>();
    widget->Create(newname, m_widgetIDs++, pos);

    widget->SetMesh(std::move(node));

    auto* pWidget = widget.get();
    m_pendingCreateQ.emplace_back(std::move(widget));

    return pWidget;
}


