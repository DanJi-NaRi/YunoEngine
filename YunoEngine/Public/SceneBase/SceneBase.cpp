#include "pch.h"

// 인클루드 순서
// 1.본인 씬 헤더
#include "SceneBase.h"

#include "IRenderer.h"
#include "YunoLight.h"
#include "YunoCamera.h"
#include "ObjectManager.h"
#include "SerializeScene.h"
#include "UIManager.h"
#include "EffectManager.h"
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

inline std::wstring Utf8ToWString(const std::string& s)
{
    if (s.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), size);
    return w;
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
    if (!m_objectManager->Init())
        return false;

    m_uiManager = std::make_unique<UIManager>();
    if (!m_uiManager->Init())
        return false;

    m_effectManager = std::make_unique<EffectManager>();
    m_effectManager->Init(50);
    m_objectManager->SetEffectManager(m_effectManager.get());

    if (m_input = YunoEngine::GetInput(); !m_input)
        return false;

    std::wstring filepath = L"../Assets/Scenes/" + Utf8ToWString(GetDebugName()) + L".json";

    SceneDesc sd;

    if (LoadScene(filepath, sd))
    {
        OnCreateScene();

        YunoEngine::GetRenderer()->GetCamera().position = ToXM(sd.camPos);
        YunoEngine::GetRenderer()->GetCamera().position = ToXM(sd.camLookAt);

        YunoEngine::GetRenderer()->SetPostProcessOption(sd.postprocess);

        m_objectManager->ProcessPending();
        if(sd.dirLight)
            m_objectManager->ApplyDirLightFromDesc(*sd.dirLight);
        m_objectManager->ApplyUnitFromDesc(sd.units);
        m_objectManager->ApplyPointLightsFromDesc(sd.pointLights);

        if (!sd.widgets.empty())
        {
            m_uiManager->ProcessPending();
            m_uiManager->ApplyWidgetFromDesc(sd.widgets);
        }

        return true;
    }

    return OnCreateScene();
    //return true;
}


bool SceneBase::LoadScene(const std::wstring& filepath, SceneDesc& out)
{
    bool res = LoadSceneFromFile(filepath, out);

    return res;
}

SceneDesc SceneBase::BuildSceneDesc()
{
    SceneDesc sd = m_objectManager->BuildSceneDesc();
    sd.sceneName = Utf8ToWString(GetDebugName());
    sd.widgets = m_uiManager->BuildWidgetDesc();
    sd.postprocess = YunoEngine::GetRenderer()->GetPostProcessOption();
    sd.camPos = FromXM(YunoEngine::GetRenderer()->GetCamera().position);
    sd.camLookAt = FromXM(YunoEngine::GetRenderer()->GetCamera().target);

    return sd;
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
    if (m_effectManager) m_effectManager->Update(dt);
}

void SceneBase::SubmitObj()
{
    if (m_objectManager) 
    {
        m_objectManager->ProcessPending();
        m_objectManager->Submit(m_lastDt);
    }
    if (m_effectManager)
    {
        m_effectManager->Submit(m_lastDt);
    }
}

void SceneBase::SubmitUI()
{
    /*if (m_uiManager)
    {
        m_uiManager->ProcessPending();
        m_uiManager->Submit(m_lastDt);
    }*/
    assert(m_uiManager);
    if (!m_uiManager) return;
    m_uiManager->ProcessPending();

    //m_uiManager->Submit(m_lastDt);    // 레이어 비적용 Submit
    m_uiManager->LayerSubmit(m_lastDt); // 레이어 적용 Submit
}


#ifdef _DEBUG
void SceneBase::DrawObjectList()
{
    if (!m_objectManager)
        return;

    if (UI::CollapsingHeader("Lights"))
    {
        auto dirLight = m_objectManager->GetDirLight();

        if (dirLight)
        {
            bool selected = dirLight == m_selectedLight;
            if (UI::Selectable("DirectionalLight", selected))
            {
                SelectLight(dirLight);
            }
        }

        auto& pointLights = m_objectManager->GetPointLights();
        if (!pointLights.empty())
        {
            int i = 0;
            for (auto& pl : pointLights)
            {
                bool selected = pl.get() == m_selectedLight;
                std::string name = "PointLight" + std::to_string(i);
                if (UI::Selectable(name.c_str(), selected))
                {
                    SelectLight(pl.get());
                }
                i++;
            }
        }
    }

    if (UI::CollapsingHeader("Objects"))
    {
        const uint32_t objcount = m_objectManager->GetObjectCount();

        auto& objlist = m_objectManager->GetObjectlist();

        for (auto [id, unit] : objlist)
        {
            Unit* obj = unit;

            if (obj->GetParent())
                continue;

            DrawObjectNode(obj);
        }
    }
    
    if (UI::CollapsingHeader("Widgets"))
    {
        if (!m_uiManager)
            return;

        const uint32_t widgetcount = m_uiManager->GetWidgetCount();

        auto& widgetlist = m_uiManager->GetWidgetlist();

        for (auto [id, widget] : widgetlist)
        {
            Widget* obj = widget;

            bool selected = false; //  EditorState에서 가져오게 될 것

            std::string name = WStringToString(obj->GetName());

            if (UI::Selectable(name.c_str(), selected))
            {
                SelectWidget(obj);
            }
        }
    }

    m_effectManager->Serialize();
}

void SceneBase::DrawObjectNode(Unit* obj)
{
    bool selected = obj == m_selectedObject;
    std::string name = WStringToString(obj->GetName());

    bool opened = UI::TreeNodeEx(obj, selected, !obj->GetChilds().empty(), name.c_str());

    if (UI::IsItemClicked())
        SelectObject(obj);

    if (opened && !obj->GetChilds().empty())
    {
        for (auto [id, child] : obj->GetChilds())
            DrawObjectNode(child);

        UI::TreePop();
    }
}

void SceneBase::DrawWidgetNode(Widget* obj)
{
    bool selected = obj == m_selectedWidget;
    std::string name = WStringToString(obj->GetName());

    bool opened = UI::TreeNodeEx(obj, selected, !obj->GetChilds().empty(), name.c_str());

    if (UI::IsItemClicked())
        SelectWidget(obj);

    if (opened && !obj->GetChilds().empty())
    {
        for (auto [id, child] : obj->GetChilds())
            DrawWidgetNode(child);

        UI::TreePop();
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
    if (m_selectedLight)
    {
        m_selectedLight->Serialize();
    }
    else if (m_selectedObject)
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
                if (UI::DragFloat3("Position##Drag", &pos.x, 0.01f, 0.0f, 0.0f, "%.2f"))
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
        if (UI::CollapsingHeader("Transform"))
        {
            auto& size = m_selectedWidget->GetSize();
            auto& pos = m_selectedWidget->GetPos();
            auto& rot = m_selectedWidget->GetRot();
            auto& scale = m_selectedWidget->GetScale();

            XMFLOAT3 degRot = rot;
            RadToDegree(degRot);

            bool isChange = false;

            if (UI::Button("Reset"))
            {
                m_selectedWidget->SetBackUpTransform();
                isChange = true;
            }

            UI::Separator();


            if (UI::DragFloat2Editable("Size", &size.x, 1.0f))
            {
                isChange = true;
            }
            UI::NextLine();
            if (UI::DragFloat3Editable("Position", &pos.x, 1.0f))
            {
                isChange = true;
            }
            if (UI::DragFloat3Editable("Rotation", &degRot.x, 0.1f))
            {
                DegreeToRad(degRot);
                rot = degRot;
                isChange = true;
            }
            if (UI::DragFloat3Editable("Scale", &scale.x, 0.1f))
            {
                isChange = true;
            }

            m_selectedWidget->Serialize();

            if (isChange) m_selectedWidget->Update();
        }
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


