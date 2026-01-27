#pragma once

#pragma once
#include "RenderTypes.h"
#include "Unit.h"
#include "AnimationUnit.h"

class YunoDirectionalLight;
class YunoPointLight;

class ObjectManager
{
private:
    size_t m_objectCount;
    UINT m_objectIDs;

    std::deque<std::unique_ptr<Unit>> m_objs;

    std::unordered_map<UINT, Unit*> m_objMap;
    std::unordered_map<std::wstring, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Unit>> m_pendingCreateQ;
   
    //Camera 투영
    bool m_isOrtho = false;

    template<typename T>
    T* CreateObject(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //재귀 오브젝트 생성용

    std::unique_ptr<MeshNode> CreateMeshNode(const std::wstring& filepath);


    std::unique_ptr<YunoDirectionalLight> m_directionLight;
    std::vector<std::unique_ptr<YunoPointLight>> m_pointLights;


public:
    void CreateDirLight();
    void CreatePointLight(const XMFLOAT3& pos, const XMFLOAT4& col, float intensity);
    void SetOrthoFlag(bool flag) { m_isOrtho = flag; };

    YunoDirectionalLight* GetDirLight() { return m_directionLight.get(); }
    std::vector<std::unique_ptr<YunoPointLight>>& GetPointLights() { return m_pointLights; }

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
    Unit* FindObject(UINT id); //id로 검색
    Unit* FindObject(const std::wstring& name); //이름으로 검색


    void DestroyObject(UINT id);
    void DestroyObject(const std::wstring& name);

    const size_t GetObjectCount() { return m_objectCount; }
    const std::unordered_map<UINT, Unit*>& GetObjectlist() { return m_objMap; }


private:
    void CheckDedicateObjectName(std::wstring& name);


    // 프레임 상수버퍼 관리
private:
    Frame_Data_Dir dirData;
    std::vector<Frame_Data_Point> plData;
    void FrameDataUpdate();
    void FrameDataSubmit();
};


template<typename T>
T* ObjectManager::CreateObject(const std::wstring& name, XMFLOAT3 pos) {
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    std::wstring newname = name;
    
    auto obj = std::make_unique<T>();
    CheckDedicateObjectName(newname);

    obj->Create(newname, m_objectIDs, pos);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));
    m_objectIDs++;

    return pObj;
}

//계층구조 오브젝트 재귀 생성용 (현재 안씀)
template<typename T>
T* ObjectManager::CreateObject(const std::wstring& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    std::wstring newname = name + L'_' + node->m_name;

    auto obj = std::make_unique<T>();
    obj->Create(newname, m_objectIDs++, pos);
    
    obj->SetMesh(std::move(node));

    auto* pObj = obj.get();
    m_pendingCreateQ.emplace_back(std::move(obj));

    return pObj;
}


template<typename T>
T* ObjectManager::CreateObjectFromFile(const std::wstring& name, XMFLOAT3 pos, const std::wstring& filepath)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    auto mesh = CreateMeshNode(filepath);

    auto meshnode = std::move(mesh);

    std::wstring newname = name;

    CheckDedicateObjectName(newname);

    auto obj = std::make_unique<T>();
    obj->Create(newname, m_objectIDs++, pos);

    obj->SetMesh(std::move(meshnode));

    auto* pObj = obj.get();
    m_pendingCreateQ.emplace_back(std::move(obj));

    return pObj;
}

