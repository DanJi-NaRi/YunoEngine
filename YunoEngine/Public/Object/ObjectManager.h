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

    void CreateObject(const std::string& name, XMFLOAT3 pos);

    //씬 매니저에 있어도 될것같은 놈들
    const Unit* FindObject(UINT id); //id로 검색
    const Unit* FindObject(const std::string& name); //이름으로 검색

    void DestroyObject(UINT id);
    void DestroyObject(const std::string& name);

    const size_t GetObjectCount() { return m_ObjectCount; }
    //여기까지

    const std::unordered_map<UINT, std::unique_ptr<Unit>>& GetObjectlist() { return m_objs; }
};
