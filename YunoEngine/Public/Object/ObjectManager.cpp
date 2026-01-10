#include "pch.h"
#include "ObjectManager.h"

ObjectManager::ObjectManager()
{
    m_ObjectCount = 0;
    m_ObjectIDs = 0;
    m_objs.reserve(30); //30개 정도 메모리 잡고 시작
    m_pendingCreateQ.reserve(30);
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Init()
{

}

void ObjectManager::Clear()
{
    m_ObjectCount = 0;
    m_ObjectIDs = 0;
    m_objs.clear(); //오브젝트 객체 완전 삭제
    m_pendingCreateQ.clear();
    m_pendingDestoryQ.clear();
    m_nameToID.clear();
}

void ObjectManager::ProcessPending()
{
    if (!m_pendingCreateQ.empty())
    {
        for (auto& obj : m_pendingCreateQ)
        {
            UINT id = obj->GetID();
            auto& name = obj->GetName();
            m_objs.insert(std::make_pair(id, std::move(obj)));
            m_nameToID.insert(std::make_pair(name, id));
            m_ObjectCount++;
        }

        m_pendingCreateQ.clear();
    }

    if (!m_pendingDestoryQ.empty())
    {
        for (auto& id : m_pendingDestoryQ)
        {
            auto& name = m_objs[id]->GetName();
            m_objs.erase(id);
            m_nameToID.erase(name);
            m_ObjectCount--;
        }

        m_pendingDestoryQ.clear();
    }
}

void ObjectManager::CreateObject(const std::string& name, XMFLOAT3 pos)
{
    m_pendingCreateQ.emplace_back(std::make_unique<Unit>(name, m_ObjectIDs, pos));
    m_ObjectIDs++;
}

//유니크 포인터의 생포인터 반환 외부에서 삭제 절대 금지
const Unit* ObjectManager::FindObject(UINT id)//GetID랑 연동해서쓰기
{
    if(m_objs.find(id) == m_objs.end())
        return nullptr;

    return m_objs[id].get();
}

const Unit* ObjectManager::FindObject(const std::string& name)
{
    if (m_nameToID.find(name) == m_nameToID.end())
        return nullptr;

    UINT id = m_nameToID[name];

    if (m_objs.find(id) == m_objs.end())
        return nullptr;

    return m_objs[id].get();
}

void ObjectManager::DestroyObject(UINT id)
{
    if (m_objs.find(id) == m_objs.end())
        return;

    for (auto& checkID : m_pendingDestoryQ)//큐에 중복삭제 삽입 방지
    {
        if (id == checkID)
            return;
    }

    m_pendingDestoryQ.push_back(id);
}

void ObjectManager::DestroyObject(const std::string& name)
{
    if (m_nameToID.find(name) == m_nameToID.end())
        return;

    UINT id = m_nameToID[name];

    for (auto& checkID : m_pendingDestoryQ)//큐에 중복삭제 삽입 방지
    {
        if (id == checkID)
            return;
    }

    m_pendingDestoryQ.push_back(id);
}
