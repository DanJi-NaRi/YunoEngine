<<<<<<< HEAD
﻿#pragma once
=======
#pragma once
#include "RenderTypes.h"
<<<<<<< HEAD
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
#include "Unit.h"



class ObjectManager
{
private:
    size_t m_ObjectCount;
    UINT m_ObjectIDs;
    std::deque<std::unique_ptr<Unit>> m_objs;
    std::unordered_map<UINT, Unit*> m_objMap;
    std::unordered_map<std::string, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Unit>> m_pendingCreateQ;

    template<typename T>
<<<<<<< HEAD
<<<<<<< HEAD
    T* CreateObject(const std::string& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //��� ������Ʈ ������

    std::unique_ptr<MeshNode> CreateMeshNode(const std::string& filepath);
=======
    Unit* CreateObject(const std::string& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //��� ������Ʈ ������
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋)
=======
    T* CreateObject(const std::string& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node); //��� ������Ʈ ������

    std::unique_ptr<MeshNode> CreateMeshNode(const std::string& filepath);
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
public:
    explicit ObjectManager();
    virtual ~ObjectManager();

    void Init(); //생성 초기화
    void Clear(); //삭제 초기화

<<<<<<< HEAD
    void ProcessPending(); //프레임 맨 마지막에 호출
=======
    void Update(float dTime);
    void Submit(float dTime);

    void Update(float dTime);
    void Submit(float dTime);

    void ProcessPending(); //������ �� �������� ȣ��
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)

    template<typename T>
<<<<<<< HEAD
<<<<<<< HEAD
    T* CreateObject(const std::string& name, XMFLOAT3 pos);
    template<typename T>
    T* CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath);
=======
    Unit* CreateObject(const std::string& name, XMFLOAT3 pos);
    template<typename T>
    Unit* CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath);
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋)

<<<<<<< HEAD
    //씬 매니저에 있어도 될것같은 놈들
    const Unit* FindObject(UINT id); //id로 검색
    const Unit* FindObject(const std::string& name); //이름으로 검색
=======
=======
    T* CreateObject(const std::string& name, XMFLOAT3 pos);
    template<typename T>
    T* CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath);

>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
    const Unit* FindObject(UINT id); //id�� �˻�
    const Unit* FindObject(const std::string& name); //�̸����� �˻�
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)

    void DestroyObject(UINT id);
    void DestroyObject(const std::string& name);

    const size_t GetObjectCount() { return m_ObjectCount; }
<<<<<<< HEAD
<<<<<<< HEAD
    //여기까지
=======
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)

    const std::unordered_map<UINT, Unit*>& GetObjectlist() { return m_objMap; }
};

template<typename T>
<<<<<<< HEAD
<<<<<<< HEAD
T* ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos) {
=======
Unit* ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos) {
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋)
=======
T* ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos) {
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");
    
    auto obj = std::make_unique<T>();
    obj->Create(name, m_ObjectIDs, pos);

    auto* pObj = obj.get();

    m_pendingCreateQ.emplace_back(std::move(obj));
    m_ObjectIDs++;

    return pObj;
<<<<<<< HEAD
}

//�������� ������Ʈ ��� ������
template<typename T>
T* ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    std::string newname = name + '_' + node->m_name;

    auto obj = std::make_unique<T>();
    obj->Create(newname, m_ObjectIDs++, pos);
    for (auto& mesh : node->m_Meshs)
    {
        obj->SetMesh(std::move(mesh));
    }

    auto* pObj = obj.get();
    m_pendingCreateQ.emplace_back(std::move(obj));

    for (auto& childNode : node->m_Childs)
    {
        auto child = CreateObject<T>(name, pos, std::move(childNode));
        pObj->Attach(child);
    }

    return pObj;
}

template<typename T>
T* ObjectManager::CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    auto meshRootNode = CreateMeshNode(filepath);
    auto pObj = CreateObject<T>(name, pos, std::move(meshRootNode));

    return pObj;
=======
}

//�������� ������Ʈ ��� ������
template<typename T>
T* ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos, std::unique_ptr<MeshNode>&& node)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    std::string newname = name + '_' + node->m_name;

    auto obj = std::make_unique<T>();
    obj->Create(newname, m_ObjectIDs++, pos);
    for (auto& mesh : node->m_Meshs)
    {
        obj->SetMesh(std::move(mesh));
    }

    auto* pObj = obj.get();
    m_pendingCreateQ.emplace_back(std::move(obj));

    for (auto& childNode : node->m_Childs)
    {
        auto child = CreateObject<T>(name, pos, std::move(childNode));
        pObj->Attach(child);
    }

    return pObj;
}

template<typename T>
T* ObjectManager::CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath)
{
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");

    auto meshRootNode = CreateMeshNode(filepath);
    auto pObj = CreateObject<T>(name, pos, std::move(meshRootNode));

<<<<<<< HEAD
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
>>>>>>> 3eb4e1a (메모할거 있어서 중간 커밋)
=======
    return pObj;
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
}
