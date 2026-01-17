#pragma once

#pragma once
#include "RenderTypes.h"
#include "Unit.h"
#include "Widget.h"

class YunoDirectionalLight;

class ObjectManager
{
private:
    size_t m_ObjectCount;
    UINT m_ObjectIDs;

    std::deque<std::unique_ptr<Unit>> m_objs;
    std::deque<std::unique_ptr<Widget>> m_widgets;

    std::unordered_map<UINT, Unit*> m_objMap;
    std::unordered_map<std::wstring, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Unit>> m_pendingCreateQ;
   

    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //재귀 오브젝트 생성용
    std::unique_ptr<MeshNode> CreateMeshNode(const std::wstring& filepath);

    std::unique_ptr<YunoDirectionalLight> m_directionLight;

public:
    void CreateDirLight();


public:
    explicit ObjectManager();
    virtual ~ObjectManager();

    bool Init(); //생성 초기화
    void Clear(); //삭제 초기화

    void ProcessPending(); //프레임 맨 마지막에 호출

    void Update(float dTime);
    void Submit(float dTime);

    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos);
    template<typename T>
    T* CreateObjectFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath);

    //씬 매니저에 있어도 될것같은 놈들
    const Unit* FindObject(UINT id); //id로 검색
    const Unit* FindObject(const std::wstring& name); //이름으로 검색


    void DestroyObject(UINT id);
    void DestroyObject(const std::wstring& name);

    const size_t GetObjectCount() { return m_ObjectCount; }
    const std::unordered_map<UINT, Unit*>& GetObjectlist() { return m_objMap; }



    // 프레임 상수버퍼 관리
private:
    Frame_Data_Dir dirData;
    void FrameDataUpdate();
    void FrameDataSubmit();
};

template<typename T>
T* ObjectManager::CreateObject(const std::wstring& name, XMFLOAT3 pos) {
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");
    
    auto obj = std::make_unique<T>();
    obj->Create(name, m_ObjectIDs, pos);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));
    m_ObjectIDs++;

    return pObj;
}

//계층구조 오브젝트 재귀 생성용
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

template<typename T>
T* ObjectManager::CreateObjectFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    auto meshRootNode = CreateMeshNode(filepath);

    if (meshRootNode)
    {
        auto pObj = CreateObject<T>(name, pos, std::move(meshRootNode));
        return pObj;
    }
        
    return nullptr;
}
