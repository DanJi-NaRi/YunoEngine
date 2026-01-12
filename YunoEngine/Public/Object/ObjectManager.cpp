<<<<<<< HEAD
﻿#include "pch.h"
=======
#include "pch.h"

>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
#include "ObjectManager.h"
#include "Parser.h"



ObjectManager::ObjectManager()
{
<<<<<<< HEAD
    m_ObjectCount = 0;
    m_ObjectIDs = 0;
    m_objs.reserve(30); //30개 정도 메모리 잡고 시작
    m_pendingCreateQ.reserve(30);
=======
    
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Init()
{
    m_ObjectCount = 0;
    m_ObjectIDs = 0;
    m_objMap.reserve(30); //30�� ���� �޸� ��� ����
    m_pendingCreateQ.reserve(30);
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


//���߿� �̺�Ʈ ť ���� �ٲ����
void ObjectManager::Update(float dTime)
{
    for (auto& obj : m_objs)
    {
        obj->Update(dTime);
    }
}
//���߿� �̺�Ʈ ť ���� �ٲ����
void ObjectManager::Submit(float dTime)
{
    for (auto& obj : m_objs)
    {
        obj->Submit(dTime);
    }
}

void ObjectManager::ProcessPending()
{
    if (!m_pendingCreateQ.empty())
    {
        for (auto& obj : m_pendingCreateQ)
        {
            UINT id = obj->GetID();
            auto name = obj->GetName();
            auto* pObj = obj.get();
            m_objs.push_back(std::move(obj));
            m_objMap.emplace(id, pObj);
            m_nameToID.emplace(name, id);
            m_ObjectCount++;
        }

        m_pendingCreateQ.clear();
    }

    if (!m_pendingDestoryQ.empty())
    {
        for (auto& id : m_pendingDestoryQ)
        {
            auto name = m_objMap[id]->GetName();

            auto it = std::find_if(m_objs.begin(), m_objs.end(), [id](const std::unique_ptr<Unit>& obj) { return obj->GetID() == id; });

            if (it == m_objs.end())
                continue;

            it->get()->DettachParent();
            it->get()->ClearChild();

            m_nameToID.erase(name);
            m_objMap.erase(id);

            m_objs.erase(it);
            m_ObjectCount--;
        }

        m_pendingDestoryQ.clear();
    }
}

//유니크 포인터의 생포인터 반환 외부에서 삭제 절대 금지
const Unit* ObjectManager::FindObject(UINT id)//GetID랑 연동해서쓰기
{
    if(m_objMap.find(id) == m_objMap.end())
        return nullptr;

    return m_objMap[id];
}

const Unit* ObjectManager::FindObject(const std::string& name)
{
    if (m_nameToID.find(name) == m_nameToID.end())
        return nullptr;

    UINT id = m_nameToID[name];

    if (m_objMap.find(id) == m_objMap.end())
        return nullptr;

    return m_objMap[id];
}

void ObjectManager::DestroyObject(UINT id)
{
    if (m_objMap.find(id) == m_objMap.end())
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

std::unique_ptr<MeshNode> ObjectManager::CreateMeshNode(const std::string& filepath)
{
    return Parser::Instance().LoadFile(filepath);
}
