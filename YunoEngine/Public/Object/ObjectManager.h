#pragma once
#include "Unit.h"
#include "Parser.h"
#include "Mesh.h"

class ObjectManager
{
private:
    size_t m_ObjectCount;
    UINT m_ObjectIDs;
    std::unordered_map<UINT, std::unique_ptr<Unit>> m_objs;
    std::unordered_map<std::string, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Unit>> m_pendingCreateQ;

    template<typename T>
    Unit* CreateObject(const std::string& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //재귀 오브젝트 생성용
public:
    explicit ObjectManager();
    virtual ~ObjectManager();

    void Init(); //생성 초기화
    void Clear(); //삭제 초기화

    void ProcessPending(); //프레임 맨 마지막에 호출

    template<typename T>
    Unit* CreateObject(const std::string& name, XMFLOAT3 pos);
    template<typename T>
    Unit* CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath);

    //씬 매니저에 있어도 될것같은 놈들
    const Unit* FindObject(UINT id); //id로 검색
    const Unit* FindObject(const std::string& name); //이름으로 검색

    void DestroyObject(UINT id);
    void DestroyObject(const std::string& name);

    const size_t GetObjectCount() { return m_ObjectCount; }
    //여기까지

    const std::unordered_map<UINT, std::unique_ptr<Unit>>& GetObjectlist() { return m_objs; }
};

template<typename T>
Unit* ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos) {
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");
    
    auto obj = std::make_unique<T>();
    obj->Create(name, m_ObjectIDs, pos);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));
    m_ObjectIDs++;

    return pObj;
}

template<typename T>
Unit* ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    auto obj = std::make_unique<T>();
    obj->Create(name, m_ObjectIDs, pos);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));
    m_ObjectIDs++;

    return pObj;
}

template<typename T>
Unit* ObjectManager::CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    auto meshNode = Parser::Instance().LoadFile(filepath);

    auto obj = CreateObject<T>(name, pos);
    for (auto& mesh : meshNode->m_Meshs)
    {
        obj->SetMesh(std::move(mesh));
    }

    for (auto& child : meshNode->m_Childs)
    {
        auto child = CreateObject(name, pos, std::move(child));
        obj->Attach(child);
    }
}
