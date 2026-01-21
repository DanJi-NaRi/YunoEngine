#include "pch.h"

#include "ObjectManager.h"
#include "Parser.h"
#include "YunoLight.h"
#include "YunoCamera.h"


void ObjectManager::CreateDirLight()
{
    m_directionLight = std::make_unique<YunoDirectionalLight>();
}

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
}

bool ObjectManager::Init()
{
    m_ObjectCount = 0;
    m_ObjectIDs = 0;
    m_objMap.reserve(30); //30개 정도 메모리 잡고 시작
    m_pendingCreateQ.reserve(30);

    return true;
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


//나중에 이벤트 큐 만들어서 바꿔야함
void ObjectManager::Update(float dTime)
{
    FrameDataUpdate();
    for (auto& obj : m_objs)
    {
        obj->Update(dTime);
    }
}
//나중에 이벤트 큐 만들어서 바꿔야함
void ObjectManager::Submit(float dTime)
{
    FrameDataSubmit();

    for (auto& obj : m_objs)
    {
        obj->Submit(dTime);
    }
}

//나중에 이벤트 큐 만들어서 바꿔야함
void ObjectManager::WidgetUpdate(float dTime)
{
    FrameDataUpdate();
    for (auto& obj : m_widgets)
    {
        obj->Update(dTime);
    }
}
//나중에 이벤트 큐 만들어서 바꿔야함
void ObjectManager::WidgetSubmit(float dTime)
{
    FrameDataSubmit();

    for (auto& widget : m_widgets)
    {
        widget->Submit(dTime);
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

void ObjectManager::ProcessWidgetPending()
{
    if (!m_widgetPendingCreateQ.empty())
    {
        for (auto& widget : m_widgetPendingCreateQ)
        {
            UINT id = widget->GetID();
            auto name = widget->GetName();
            auto* pWidget = widget.get();
            m_widgets.push_back(std::move(widget));
            m_WidgetMap.emplace(id, pWidget);
            m_widgetNameToID.emplace(name, id);
            m_WidgetCount++;
        }

        m_widgetPendingCreateQ.clear();
    }

    if (!m_widgetPendingDestoryQ.empty())
    {
        for (auto& id : m_widgetPendingDestoryQ)
        {
            auto name = m_WidgetMap[id]->GetName();

            auto it = std::find_if(m_widgets.begin(), m_widgets.end(), [id](const std::unique_ptr<Widget>& widget) { return widget->GetID() == id; });

            if (it == m_widgets.end())
                continue;

            // 임시 주석 처리
            //it->get()->DettachParent();
            //it->get()->ClearChild();

            m_widgetNameToID.erase(name);
            m_WidgetMap.erase(id);

            m_widgets.erase(it);
            m_WidgetCount--;
        }

        m_widgetPendingDestoryQ.clear();
    }
}

//유니크 포인터의 생포인터 반환 외부에서 삭제 절대 금지
const Unit* ObjectManager::FindObject(UINT id)//GetID랑 연동해서쓰기
{
    if(m_objMap.find(id) == m_objMap.end())
        return nullptr;

    return m_objMap[id];
}

const Unit* ObjectManager::FindObject(const std::wstring& name)
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

void ObjectManager::DestroyObject(const std::wstring& name)
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

void ObjectManager::CheckDedicateObjectName(std::wstring& name)
{
    int count = 0;

    for (auto& obj : m_pendingCreateQ)
    {
        if (obj->GetName().find(name) != std::wstring::npos)
            count++;
    }

    for (auto& [id, obj] : m_objMap)
    {
        if (obj->GetName().find(name) != std::wstring::npos)
            count++;
    }

    if (count)
        name += std::to_wstring(count);
}

void ObjectManager::FrameDataUpdate()
{
    // 오브젝트 매니저가 라이트를 가지고 있는데
    // 이 라이트는 씬에서 만들어서 넘겨주는?
    // 디렉션 1개만 포인트 스팟 >> 벡터
    if (m_directionLight) // 라이트가 있으면 업데이트
    {
        dirData.Lightdir = m_directionLight->GetDirFloat4Reverse();
        dirData.Lightdiff = m_directionLight->GetDiffFloat4();
        dirData.Lightamb = m_directionLight->GetAmbFloat4();
        dirData.Lightspec = m_directionLight->GetSpecFloat4();
        dirData.intensity = m_directionLight->GetIntensity();
    }

}

void ObjectManager::FrameDataSubmit()
{
    // 여기서 서브밋할 예정
    auto* renderer = YunoEngine::GetRenderer();
    renderer->GetCamera().SetOrthoFlag(m_isOrtho);

    renderer->BindConstantBuffers_Camera(dirData);

    if (m_directionLight)// 라이트가 있으면 프레임 데이터 넘기기
    {
        renderer->BindConstantBuffers_Light(dirData);
    }

}

std::unique_ptr<MeshNode> ObjectManager::CreateMeshNode(const std::wstring& filepath)
{
    return Parser::Instance().LoadFile(filepath);
}
