#include "pch.h"

// 인클루드 순서
// 1.본인 씬 헤더
#include "SceneBase.h"

#include "IRenderer.h"
#include "ObjectManager.h"
#include "UImgui.h"

std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.data(), (int)wstr.size(),
        nullptr, 0,
        nullptr, nullptr
    );

    std::string result(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.data(), (int)wstr.size(),
        result.data(), size_needed,
        nullptr, nullptr
    );

    return result;
}

SceneBase::SceneBase() = default;
SceneBase::~SceneBase() = default;

// 생성
bool SceneBase::OnCreate()
{
    //std::cout << "[SceneBase] OnCreate\n";
    m_name = "SceneBase";

    m_objectManager = std::make_unique<ObjectManager>();
    if (!m_objectManager)
        return false;

    if (!m_objectManager->Init())
        return false;

    return OnCreateScene();
}

// 삭제
void SceneBase::OnDestroy()
{
    //std::cout << "[SceneBase] OnDestroy\n";

    OnDestroyScene();

    if (m_objectManager)
    {
        m_objectManager->Clear();
        m_objectManager.reset();
    }
}

// 진입
void SceneBase::OnEnter()
{
    //std::cout << "[SceneBase] OnEnter\n";
}

// 진출
void SceneBase::OnExit()
{
    //std::cout << "[SceneBase] OnExit\n";
}




void SceneBase::Update(float dt)
{
    m_lastDt = dt;

    if (m_objectManager)
    {
        m_objectManager->WidgetUpdate(dt);
        m_objectManager->Update(dt);
    }
        
        
}

void SceneBase::Submit()
{
    if (m_objectManager) 
    {
        m_objectManager->Submit(m_lastDt);
        m_objectManager->ProcessPending();

        m_objectManager->WidgetSubmit(m_lastDt);
        m_objectManager->ProcessWidgetPending();
    }

}

void SceneBase::DrawObjectListUI()
{
    if (!m_objectManager)
        return;

    UI::SetNextUIPos(800, 0);
    UI::BeginPanel("ObjectHierarchy");

    const uint32_t objcount = m_objectManager->GetObjectCount();
    const uint32_t widgetcount = m_objectManager->GetWidgetCount();

    auto& objlist = m_objectManager->GetObjectlist();
    auto& widgetlist = m_objectManager->GetWidgetlist();

    for (uint32_t i = 0; i < objcount; ++i)
    {
        auto it = objlist.find(i);
        if(it == objlist.end())
            continue;

        const Unit* obj = it->second;

        bool selected = false; // ← EditorState에서 가져오게 될 것

        std::string name = WStringToString(obj->GetName());

        if (UI::Selectable(name.c_str(), selected))
        {
            // 여기서는 “선택 요청”만 보냄
            // 실제 선택 상태 저장은 Engine/EditorState에서
        }
    }

    for (uint32_t i = 0; i < widgetcount; ++i)
    {
        auto it = widgetlist.find(i);
        if (it == widgetlist.end())
            continue;

        const Widget* obj = it->second;

        bool selected = false; // ← EditorState에서 가져오게 될 것

        std::string name = WStringToString(obj->GetName());

        if (UI::Selectable(name.c_str(), selected))
        {
            // 여기서는 “선택 요청”만 보냄
            // 실제 선택 상태 저장은 Engine/EditorState에서
        }
    }

    UI::EndPanel();
}

bool SceneBase::OnCreateScene()
{
    // 각 씬에서 여기에 씬 생성시 할 행동 구현하면 됨

    return true;
}

void SceneBase::OnDestroyScene()
{
    // 각 씬에서 여기에 씬 파괴시 할 행동 구현하면 됨
}
