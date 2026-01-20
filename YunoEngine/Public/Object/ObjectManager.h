#pragma once

#pragma once
#include "RenderTypes.h"
#include "Unit.h"
#include "Widget.h"
#include "AnimationUnit.h"

class YunoDirectionalLight;

class ObjectManager
{
private:
    size_t m_ObjectCount;
    size_t m_WidgetCount;
    UINT m_ObjectIDs;

    std::deque<std::unique_ptr<Unit>> m_objs;

    std::unordered_map<UINT, Unit*> m_objMap;
    std::unordered_map<std::wstring, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Unit>> m_pendingCreateQ;
   
    // Widgets
    std::deque<std::unique_ptr<Widget>> m_widgets;
    std::unordered_map<UINT, Widget*> m_WidgetMap;
    std::unordered_map<std::wstring, UINT> m_widgetNameToID;

    std::vector<UINT> m_widgetPendingDestoryQ;
    std::vector<std::unique_ptr<Widget>> m_widgetPendingCreateQ;

    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //재귀 오브젝트 생성용
    std::pair<std::unique_ptr<MeshNode>, std::unique_ptr<Animator>> CreateMeshNode(const std::wstring& filepath);

    std::unique_ptr<YunoDirectionalLight> m_directionLight;

    template<typename T>
    T* CreateWidget(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //재귀 위젯 생성용

public:
    void CreateDirLight();


public:
    explicit ObjectManager();
    virtual ~ObjectManager();

    bool Init(); //생성 초기화
    void Clear(); //삭제 초기화

    void ProcessPending(); //프레임 맨 마지막에 호출
    void ProcessWidgetPending();

    void Update(float dTime);
    void Submit(float dTime);
    void WidgetUpdate(float dTime);
    void WidgetSubmit(float dTime);

    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos);
    template<typename T>
    T* CreateObjectFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath);

    template<typename T>
    T* CreateWidget(const std::wstring& name, XMFLOAT3 pos);
    template<typename T>
    T* CreateWidgetFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath);

    //씬 매니저에 있어도 될것같은 놈들
    const Unit* FindObject(UINT id); //id로 검색
    const Unit* FindObject(const std::wstring& name); //이름으로 검색


    void DestroyObject(UINT id);
    void DestroyObject(const std::wstring& name);

    const size_t GetObjectCount() { return m_ObjectCount; }
    const size_t GetWidgetCount() { return m_WidgetCount; }
    const std::unordered_map<UINT, Unit*>& GetObjectlist() { return m_objMap; }
    const std::unordered_map<UINT, Widget*>& GetWidgetlist() { return m_WidgetMap; }

private:
    void CheckDedicateObjectName(std::wstring& name);


    // 프레임 상수버퍼 관리
private:
    Frame_Data_Dir dirData;
    void FrameDataUpdate();
    void FrameDataSubmit();
};

template<typename T>
T* ObjectManager::CreateObject(const std::wstring& name, XMFLOAT3 pos) {
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    std::wstring newname = name;
    
    auto obj = std::make_unique<T>();
    CheckDedicateObjectName(newname);

    obj->Create(newname, m_ObjectIDs, pos);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));
    m_ObjectIDs++;

    return pObj;
}

//계층구조 오브젝트 재귀 생성용 (현재 안씀)
template<typename T>
T* ObjectManager::CreateObject(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    std::wstring newname = name + L'_' + node->m_name;

    auto obj = std::make_unique<T>();
    obj->Create(newname, m_ObjectIDs++, pos);
    
    obj->SetMesh(std::move(node));

    auto* pObj = obj.get();
    m_pendingCreateQ.emplace_back(std::move(obj));

    return pObj;
}


// 김장후 - 
// 현재 Object와 같은 ID 체계를 사용하고 있음. 분리할지 고민중..
// 매니저 분리까지는 굳이 필요 없을 것 같아서 일단은 큐만 추가해서 사용 중.
template<typename T>
T* ObjectManager::CreateWidget(const std::wstring& name, XMFLOAT3 pos)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");

    auto widget = std::make_unique<T>();
    if (!widget->Create(name, m_ObjectIDs, pos))
        return nullptr;

    auto* pWidget = widget.get();
    m_widgetPendingCreateQ.emplace_back(std::move(widget));
    ++m_ObjectIDs;
    return pWidget;
}

// 계층구조 오브젝트 재귀 생성용
template<typename T>
T* ObjectManager::CreateWidget(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    if (!node) return nullptr;

    std::wstring newname = name + L'_' + node->m_name;

    auto widget = std::make_unique<T>();
    if (!widget->Create(newname, m_ObjectIDs, pos))
        return nullptr;

    widget->SetMesh(std::move(node));

    auto* pWidget = widget.get();
    m_widgetPendingCreateQ.emplace_back(std::move(widget));
    ++m_ObjectIDs;
    return pWidget;
}


template<typename T>
T* ObjectManager::CreateObjectFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    auto meshAndAnim = CreateMeshNode(filepath);

    auto meshnode = std::move(meshAndAnim.first);
    auto animator = std::move(meshAndAnim.second);

    std::wstring newname = name;

    CheckDedicateObjectName(newname);

    auto obj = std::make_unique<T>();
    obj->Create(newname, m_ObjectIDs++, pos);

    obj->SetMesh(std::move(meshnode));

    if constexpr (std::is_base_of_v<AnimationUnit, T>)
        if (animator)
        {
            obj->SetAnimator(std::move(animator));
        }

    auto* pObj = obj.get();
    m_pendingCreateQ.emplace_back(std::move(obj));

    return pObj;
}

template<typename T>
T* ObjectManager::CreateWidgetFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must Derived Widget(UIObject, ObjectManager.h)");

    auto meshRootNode = CreateMeshNode(filepath);

    if (meshRootNode)
    {
        auto pWidget = CreateWidget<T>(name, pos, std::move(meshRootNode));
        return pWidget;
    }

    return nullptr;
}

