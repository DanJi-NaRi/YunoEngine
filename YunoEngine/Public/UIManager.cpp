#include "pch.h"

#include "UIManager.h"
#include "Parser.h"
#include "YunoLight.h"
#include "YunoCamera.h"
#include "YunoInputSystem.h"



void UIManager::CreateDirLight()
{
    m_directionLight = std::make_unique<YunoDirectionalLight>();
}

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

bool UIManager::Init()
{
    m_widgetCount = 0;
    m_widgetIDs = 0;
    m_widgetMap.reserve(30); //30개 정도 메모리 잡고 시작
    m_pendingCreateQ.reserve(30);
    m_pInput = YunoEngine::GetInput();
    return true;
}

void UIManager::Clear()
{

    m_widgetCount = 0;
    m_widgetIDs = 0;
    m_widgets.clear(); //오브젝트 객체 완전 삭제
    m_pendingCreateQ.clear();
    m_pendingDestoryQ.clear();
    m_nameToID.clear();
}


//나중에 이벤트 큐 만들어서 바꿔야함
void UIManager::Update(float dTime)
{
    FrameDataUpdate();
    UpdateButtonStates(); // 모든 버튼 상태 업데이트
    for (auto& widget : m_widgets)
    {
        widget->Update(dTime);
    }
}
//나중에 이벤트 큐 만들어서 바꿔야함
void UIManager::Submit(float dTime)
{
    FrameDataSubmit();

    for (auto& widget : m_widgets)
    {
        widget->Submit(dTime);
    }
}

void UIManager::ProcessPending()
{
    if (!m_pendingCreateQ.empty())
    {
        for (auto& widget : m_pendingCreateQ)
        {
            UINT id = widget->GetID();
            auto name = widget->GetName();
            auto* pWidget = widget.get();
            m_widgets.push_back(std::move(widget));
            m_widgetMap.emplace(id, pWidget);
            m_nameToID.emplace(name, id);
            m_widgetCount++;
        }

        m_pendingCreateQ.clear();
    }

    if (!m_pendingDestoryQ.empty())
    {
        for (auto& id : m_pendingDestoryQ)
        {
            auto name = m_widgetMap[id]->GetName();

            auto it = std::find_if(m_widgets.begin(), m_widgets.end(), [id](const std::unique_ptr<Widget>& widget) { return widget->GetID() == id; });

            if (it == m_widgets.end())
                continue;

            it->get()->DettachParent();
            it->get()->ClearChild();

            m_nameToID.erase(name);
            m_widgetMap.erase(id);

            m_widgets.erase(it);
            m_widgetCount--;
        }

        m_pendingDestoryQ.clear();
    }
}

//유니크 포인터의 생포인터 반환 외부에서 삭제 절대 금지
const Widget* UIManager::FindWidget(UINT id)//GetID랑 연동해서쓰기
{
    if (m_widgetMap.find(id) == m_widgetMap.end())
        return nullptr;

    return m_widgetMap[id];
}

const Widget* UIManager::FindWidget(const std::wstring& name)
{
    if (m_nameToID.find(name) == m_nameToID.end())
        return nullptr;

    UINT id = m_nameToID[name];

    if (m_widgetMap.find(id) == m_widgetMap.end())
        return nullptr;

    return m_widgetMap[id];
}

void UIManager::DestroyWidget(UINT id)
{
    if (m_widgetMap.find(id) == m_widgetMap.end())
        return;

    for (auto& checkID : m_pendingDestoryQ)//큐에 중복삭제 삽입 방지
    {
        if (id == checkID)
            return;
    }

    m_pendingDestoryQ.push_back(id);
}

void UIManager::DestroyWidget(const std::wstring& name)
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

void UIManager::UpdateButtonStates() // 기본 상태 (Idle,Hover) 업데이트
{
    assert(m_pInput);

    POINT mouseXY{ (LONG)m_pInput->GetMouseX(), (LONG)m_pInput->GetMouseY() };

    Button* Btn = nullptr;

    // 버튼 : 마우스 입력 상태 확인
    for (auto& widget : m_widgets)
    {
        if (Btn = dynamic_cast<Button*>(widget.get())) { // 클릭 가능(버튼 파생)일 경우
            Btn->SetButtonState(ButtonState::Idle); // 커서 영역 검사 전 기본 상태 초기화.
        }

        if (!widget->IsCursorOverWidget(mouseXY)) { // 커서가 위젯 위에 있는지 체크
            if (Btn) Btn->IdleEvent();
            Btn = nullptr;
            continue;
        }

        if (Btn) {
            Btn->SetButtonState(ButtonState::Hovered);
            Btn->HoveredEvent();
            //std::cout << "Hovered!!" << std::endl;
        }
        Btn = nullptr; // 다음 검사를 위해 초기화
    }
}



void UIManager::CheckDedicateWidgetName(std::wstring& name)
{
    int count = 0;

    for (auto& obj : m_pendingCreateQ)
    {
        if (obj->GetName().find(name) != std::wstring::npos)
            count++;
    }

    for (auto& [id, obj] : m_widgetMap)
    {
        if (obj->GetName().find(name) != std::wstring::npos)
            count++;
    }

    if (count)
        name += std::to_wstring(count);
}

void UIManager::FrameDataUpdate()
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

void UIManager::FrameDataSubmit()
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

std::pair<std::unique_ptr<MeshNode>, std::unique_ptr<Animator>> UIManager::CreateMeshNode(const std::wstring& filepath)
{
    return Parser::Instance().LoadFile(filepath);
}
