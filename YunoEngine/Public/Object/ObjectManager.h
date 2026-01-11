#pragma once
#include "Unit.h"

class ObjectManager
{
private:
    size_t m_ObjectCount;
    UINT m_ObjectIDs;
    std::unordered_map<UINT, std::unique_ptr<Unit>> m_objs;
    std::unordered_map<std::string, UINT> m_nameToID;

    std::vector<UINT> m_pendingDestoryQ;
    std::vector<std::unique_ptr<Unit>> m_pendingCreateQ;
public:
    explicit ObjectManager();
    virtual ~ObjectManager();

    void Init(); //생성 초기화
    void Clear(); //삭제 초기화

    void ProcessPending(); //프레임 맨 마지막에 호출

    template<typename T>
    void CreateObject(const std::string& name, XMFLOAT3 pos);
    template<typename T>
    void CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath);

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
void ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos) {
    static_assert(std::is_base_of_v<Unit, T>, "T must Derived Unit(GameObject, ObjectManager.h)");
    
    auto obj = std::make_unique<T>();
    obj->Create(name, m_ObjectIDs, pos);

    m_pendingCreateQ.emplace_back(std::move(obj));
    m_ObjectIDs++;
}

template<typename T>
void CreateObjectFromFile(const std::string& name, XMFLOAT3 pos, const std::string& filepath)
{

}
