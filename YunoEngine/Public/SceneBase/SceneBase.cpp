#include "pch.h"

// 인클루드 순서
// 1.본인 씬 헤더
#include "SceneBase.h"

#include "IRenderer.h"
#include "ObjectManager.h"
#include "UIManager.h"
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

#ifdef _DEBUG
    m_selectedObject = nullptr;
    m_selectedWidget = nullptr;
#endif


    m_objectManager = std::make_unique<ObjectManager>();
    if (!m_objectManager)
        return false;

    m_uiManager = std::make_unique<UIManager>();
    if (!m_objectManager->Init())
        return false;

    if (m_input = YunoEngine::GetInput(); !m_input)
        return false;

    if (!m_uiManager->Init())
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

    if (m_uiManager)
    {
        m_uiManager->Clear();
        m_uiManager.reset();
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

    if (m_objectManager) m_objectManager->Update(dt);
    if (m_uiManager)     m_uiManager->Update(dt);

}

void SceneBase::SubmitObj()
{
    if (m_objectManager) 
    {
        m_objectManager->ProcessPending();
        m_objectManager->Submit(m_lastDt);
    }
}

void SceneBase::SubmitUI()
{
    if (m_uiManager)
    {
        m_uiManager->ProcessPending();
        m_uiManager->Submit(m_lastDt);
    }
}

#ifdef _DEBUG
void SceneBase::DrawObjectList()
{
    if (!m_objectManager)
        return;

    const uint32_t objcount = m_objectManager->GetObjectCount();

    auto& objlist = m_objectManager->GetObjectlist();


    for (uint32_t i = 0; i < objcount; ++i)
    {
        auto it = objlist.find(i);
        if(it == objlist.end())
            continue;

        Unit* obj = it->second;

        bool selected = false; // ← EditorState에서 가져오게 될 것

        std::string name = WStringToString(obj->GetName());

        if (UI::Selectable(name.c_str(), selected))
        {
            SelectObject(obj);
        }
    }
}

void SceneBase::DrawUIList()
{
    if (!m_uiManager)
        return;

    const uint32_t widgetcount = m_uiManager->GetWidgetCount();

    auto& widgetlist = m_uiManager->GetWidgetlist();

    for (uint32_t i = 0; i < widgetcount; ++i)
    {
        auto it = widgetlist.find(i);
        if (it == widgetlist.end())
            continue;

        Widget* obj = it->second;

        bool selected = false; //  EditorState에서 가져오게 될 것

        std::string name = WStringToString(obj->GetName());

        if (UI::Selectable(name.c_str(), selected))
        {
            SelectWidget(obj);
        }
    }
}


void RadToDegree(XMFLOAT3& out)
{
    out.x = XMConvertToDegrees(out.x);
    out.y = XMConvertToDegrees(out.y);
    out.z = XMConvertToDegrees(out.z);
}

void DegreeToRad(XMFLOAT3& out)
{
    out.x = XMConvertToRadians(out.x);
    out.y = XMConvertToRadians(out.y);
    out.z = XMConvertToRadians(out.z);
}



void SceneBase::DrawInspector()
{
    if (m_selectedObject)
    {
        if (UI::CollapsingHeader("Transform"))
        {
            auto& pos = m_selectedObject->GetPos();
            auto& rot = m_selectedObject->GetRot();
            auto& scale = m_selectedObject->GetScale();

            XMFLOAT3 degRot = rot;
            RadToDegree(degRot);

            bool isChange = false;

            static bool editPos = false;
            static bool editRot = false;
            static bool editScale = false;
            static float value = 0.0f;


            if (UI::Button("Reset"))
            {
                m_selectedObject->SetBackUpTransform();
                isChange = true;
            }

            UI::Separator();

            if (!editPos)
            {
                if (UI::DragFloat3("Position##Drag", &pos.x, 0.1f))
                {
                    isChange = true;
                }

                if (UI::IsItemHovered() &&
                    UI::IsLeftMouseDoubleClicked())
                {
                    editPos = true;
                }
            }
            else
            {
                if (UI::InputFloat3("Position##Input", &pos.x))
                {
                    isChange = true;
                }

                if (UI::IsItemDeactivatedAfterEdit())
                {
                    editPos = false;
                }
            }
            
            if (!editRot)
            {
                if (UI::DragFloat3("Rotation##Drag", &degRot.x, 0.5f))
                {
                    DegreeToRad(degRot);
                    m_selectedObject->SetRot(degRot);

                    isChange = true;
                }

                if (UI::IsItemHovered() &&
                    UI::IsLeftMouseDoubleClicked())
                {
                    editRot = true;
                }
            }
            else
            {
                if (UI::InputFloat3("Rotation##Input", &degRot.x))
                {
                    DegreeToRad(degRot);
                    m_selectedObject->SetRot(degRot);
                    isChange = true;
                }

                if (UI::IsItemDeactivatedAfterEdit())
                {
                    editRot = false;
                }
            }
            
            if (!editScale)
            {
                if (UI::DragFloat3("Scale##Drag", &scale.x, 0.1f, 0.001f, 1000.0f))
                {
                    isChange = true;
                }

                if (UI::IsItemHovered() &&
                    UI::IsLeftMouseDoubleClicked())
                {
                    editScale = true;
                }
            }
            else
            {
                if (UI::InputFloat3("Scale##Input", &scale.x))
                {
                    isChange = true;
                }

                if (UI::IsItemDeactivatedAfterEdit())
                {
                    editScale = false;
                }
            }
            
            m_selectedObject->Serialize();

            if (isChange) m_selectedObject->Update();
        }
    }
    else if(m_selectedWidget)
    {
       
    }
}
#endif


bool SceneBase::OnCreateScene()
{
    // 각 씬에서 여기에 씬 생성시 할 행동 구현하면 됨

    return true;
}

void SceneBase::OnDestroyScene()
{
    // 각 씬에서 여기에 씬 파괴시 할 행동 구현하면 됨
}
