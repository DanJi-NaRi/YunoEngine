#include "pch.h"

#include "SerializeScene.h"
#include "UIManager.h"
#include "Parser.h"
#include "YunoLight.h"
#include "YunoCamera.h"
#include "YunoTransform.h"
#include "IInput.h"
#include "UIFactory.h"
#include "Button.h"


UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

bool UIManager::Init()
{
    m_uiFactory = std::make_unique<UIFactory>(*this);
    m_widgetCount = 0;
    m_widgetIDs = 0;
    m_widgetMap.reserve(30); //30개 정도 메모리 잡고 시작
    m_pendingCreateQ.reserve(30);
    m_pInput = YunoEngine::GetInput();
    m_cursurSystem.Init(&m_widgets); // 커서 시스템에 위젯 정보 넘기기
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
        if(widget->GetIsRoot()) widget->UpdateTransform(dTime); // 체이닝
        widget->UpdateLogic(dTime); // 로직 업데이트 // 체이닝 금지
    }
}
//나중에 이벤트 큐 만들어서 바꿔야함
void UIManager::Submit(float dTime)
{
    FrameDataSubmit();
    
    // 우선순위에 따라서 정렬

    for (auto& widget : m_widgets)
    {
        if (widget->GetIsRoot()) // 체이닝
            widget->Submit(dTime);
    }
}

void UIManager::GetSurface()
{

}

void UIManager::ProcessPending()
{
    std::vector<Widget*> newWidgets;
    newWidgets.reserve(m_pendingCreateQ.size());

    if (!m_pendingCreateQ.empty())
    {
        for (auto& widget : m_pendingCreateQ)
        {
            UINT id = widget->GetID();
            auto name = widget->GetName();
            auto* pWidget = widget.get();
            // 시작 벡터
            newWidgets.push_back(pWidget); // StartPtr 백업

            m_widgets.push_back(std::move(widget));
            m_widgetMap.emplace(id, pWidget);
            m_nameToID.emplace(name, id);
            m_widgetCount++;
        }
        m_pendingCreateQ.clear();
    }

    for (Widget* widget : newWidgets)
    {
        if (widget) widget->Start(); // Start 실행
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
Widget* UIManager::FindWidget(UINT id)//GetID랑 연동해서쓰기
{
    if (m_widgetMap.find(id) == m_widgetMap.end())
        return nullptr;

    return m_widgetMap[id];
}

Widget* UIManager::FindWidget(const std::wstring& name)
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
        if (Btn = dynamic_cast<Button*>(widget.get()); !Btn) continue;

        Btn->SetButtonState(ButtonState::Idle); // 초기화


        ////////////////// 키보드 입력 검사 단계 //////////////////

        if (Btn->IsBindkey()) { // 바인딩 키가 있을 경우
            uint32_t key = Btn->GetBindKey();
            if (m_pInput->IsKeyPressed(key)) {
                Btn->SetButtonState(ButtonState::Pressed);
                //std::cout << "KeyPressed!!" << std::endl;
                continue; // 키 입력이 있었다면, 스킵
            }
            else if (m_pInput->IsKeyDown(key)) {
                Btn->SetButtonState(ButtonState::Down);
                //std::cout << "KeyDown!!" << std::endl;
                continue; // 키 입력이 있었다면, 스킵
            }
            else if (m_pInput->IsKeyReleased(key)) {
                Btn->SetButtonState(ButtonState::Released);
                //std::cout << "KeyReleased!!" << std::endl;
                continue; // 키 입력이 있었다면, 스킵
            }
        }

        ////////////////// 마우스 입력 검사 단계 //////////////////

        if (!widget->IsCursorOverWidget(mouseXY)) { // 커서가 위젯 위에 있는지 체크
            Btn = nullptr;
            continue;
        }
        else if (m_pInput->IsMouseButtonPressed(0)) {
            Btn->SetButtonState(ButtonState::Pressed);
            m_cursurSystem.SetFocusedWidget(Btn); // 마지막으로 누른 버튼 갱신
            m_cursurSystem.SetFocusedMouseButton(0);
            //Btn->PressedEvent();
            //std::cout << "LMBPressed!!" << std::endl;
        }
        else if (m_pInput->IsMouseButtonPressed(1)) {
            Btn->SetButtonState(ButtonState::Pressed);
            m_cursurSystem.SetFocusedWidget(Btn); // 마지막으로 누른 버튼 갱신
            m_cursurSystem.SetFocusedMouseButton(1);
            //Btn->PressedEvent();
            //std::cout << "RMBPressed!!" << std::endl;
        }
        else if (m_pInput->IsMouseButtonDown(0) || m_pInput->IsMouseButtonDown(1)) {
            Btn->SetButtonState(ButtonState::Down);
            //Btn->DownEvent();
            //std::cout << "Down!!" << std::endl;
        }
        //else if (m_pInput->IsMouseButtonReleased(0)) {
        //    Btn->SetButtonState(ButtonState::Released);
        //    //Btn->ReleasedEvent();
        //    //std::cout << "LMBReleased!!" << std::endl;
        //}
        //else if (m_pInput->IsMouseButtonReleased(1)) {
        //    Btn->SetButtonState(ButtonState::Released);
        //    //Btn->ReleasedEvent();
        //    //std::cout << "RMBReleased!!" << std::endl;
        //}
        else {
            Btn->SetButtonState(ButtonState::Hovered);
            //Btn->HoveredEvent();
            //std::cout << "Hovered!!" << std::endl;
        }
        Btn = nullptr; // 다음 검사를 위해 초기화
    }
}

bool UIManager::ProcessButtonMouse(ButtonState state, uint32_t mouseButton)
{
    assert(m_pInput);

    // 1) 이번 프레임 입력 발생 체크
    switch (state) {
    case ButtonState::Pressed:
        if (!m_pInput->IsMouseButtonPressed(mouseButton)) return false;
        break;
    case ButtonState::Down:
        if (!m_pInput->IsMouseButtonDown(mouseButton)) return false;
        break;
    case ButtonState::Released:
    {
        if (!m_pInput->IsMouseButtonReleased(mouseButton)) return false;

        // 커서가 밖이어도 포커스된 버튼이면 릴리즈 보장

        Button* focusWidget = m_cursurSystem.GetFocusedWidget();
        if (focusWidget)
        {
            if (m_cursurSystem.GetFocusedMouseButton() != mouseButton) return false;

            focusWidget->SetButtonState(ButtonState::Released);

            if (mouseButton == 0) {
                m_cursurSystem.FindSnapWidget(); // DragProvider가 있을 경우, 스냅 검색 // LMBReleasedEvent와 순서 주의. IsDrag가 여기서 처리됨.
                focusWidget->LMBReleasedEvent();
            }
            else if (mouseButton == 1)
            {
                focusWidget->RMBReleasedEvent();
            }
            m_cursurSystem.SetFocusedWidget(nullptr);
            focusWidget = nullptr;
            return true;
        }
        break;
    }
    default:
        // Hover/Idle는 "입력 발생" 기반 함수가 아님
        return false;
    }

    // 2) 상태 갱신
    UpdateButtonStates();

    POINT mouseXY{ (LONG)m_pInput->GetMouseX(), (LONG)m_pInput->GetMouseY() };

    // 3) 커서 안의 버튼 중 state가 동일하면 실행
    for (auto& widget : m_widgets)
    {
        Button* Btn = dynamic_cast<Button*>(widget.get());
        if (!Btn) continue; // 버튼인지

        if (Btn->GetButtonState() != state) continue; // 버튼과 State가 동일한지

        // 커서가 위젯 위에 있는 지 검사
        if (!widget->IsCursorOverWidget(mouseXY)) continue;

        switch (state) {
        case ButtonState::Pressed:  
            if(mouseButton == 0) Btn->LMBPressedEvent();
            else if(mouseButton == 1) Btn->RMBPressedEvent();
            return true;
        case ButtonState::Down:     
            Btn->DownEvent();     
            return true;
        //case ButtonState::Released: // 사실상 이전에 처리
        //    if (mouseButton == 0) Btn->LMBReleasedEvent();
        //    else if (mouseButton == 1) Btn->RMBReleasedEvent(); 
        //    return true;
        default: return false;
        }
    }

    return false;
}

bool UIManager::ProcessButtonKey(ButtonState state, uint32_t key)
{
    // 1) 이번 프레임 입력 발생 체크
    switch (state) {
    case ButtonState::Pressed:
        if (!m_pInput->IsKeyPressed(key)) return false;
        break;
    case ButtonState::Down:
        if (!m_pInput->IsKeyDown(key)) return false;
        break;
    case ButtonState::Released:
    {
        if (!m_pInput->IsKeyReleased(key)) return false;
        Button* usekeyWidget = m_cursurSystem.GetUseKeyWidget();
        if (usekeyWidget)
        {
            if (m_cursurSystem.GetUseKeyWidgetBindKey() != key) return false;
            usekeyWidget->SetButtonState(ButtonState::Released);
            usekeyWidget->KeyReleasedEvent(key);
            m_cursurSystem.SetUseKeyWidget(nullptr);
            usekeyWidget = nullptr;
            return true;
            break;
        }
    }
    default:
        return false;
    }

    // 2단계 : 입력이 발생한 프레임이므로, 상태 갱신
    UpdateButtonStates();

    // 3) 바인딩 키가 같은 버튼 중 state가 동일하면 실행
    for (auto& widget : m_widgets)
    {
        Button* Btn = dynamic_cast<Button*>(widget.get());
        if (!Btn) continue;

        if (!Btn->IsBindkey()) continue;
        if (Btn->GetBindKey() != key) continue;

        if (Btn->GetButtonState() != state) continue;

        switch (state) {
        case ButtonState::Pressed:  Btn->KeyPressedEvent(key);  return true;
        case ButtonState::Down:     Btn->DownEvent();     return true;
            //case ButtonState::Released: Btn->KeyReleasedEvent(key); return true;
        default: return false;
        }
    }
    return false;
}

// 아직 캔버스 개념이 없으므로 클라이언트가 곧 캔버스임. (단일 캔버스 느낌..)

Float2 UIManager::GetCanvasSize() // 개선사항 : 멤버에 this라던가 위젯 식별자를 넣고
{
    // 부모를 따라가다가 부모가 가진 Canvas 정보가 있다면 최상위 부모의 Canvas가 있다면, if로 해당 sizeXY 반환
    
    //최상위 부모가 캔버스 타입이 아니면
    return Float2(YunoEngine::GetWindow()->GetClientWidth(), YunoEngine::GetWindow()->GetClientHeight());
}
// 아직 캔버스 개념이 없으므로 클라이언트가 곧 캔버스임. (단일 캔버스 느낌..)


std::vector<WidgetDesc> UIManager::BuildWidgetDesc()
{
    std::vector<WidgetDesc> wds;
    for (auto& w : m_widgets)
    {
        WidgetDesc wd;
        wd = w->BuildWidgetDesc();

        wds.push_back(wd);
    }
    return wds;
}

void UIManager::ApplyWidgetFromDesc(const std::vector<WidgetDesc>& wds)
{
    for (auto& d : wds)
    {
        Widget* w = FindWidget(d.name);

        if (!w) continue;

        XMFLOAT3 radRot = { XMConvertToRadians(d.transform.rotation.x), XMConvertToRadians(d.transform.rotation.y), XMConvertToRadians(d.transform.rotation.z) };

        w->SetPos(ToXM(d.transform.position));
        w->SetRot(radRot);
        w->SetScale(ToXM(d.transform.scale));
    }
}

void UIManager::CheckDedicateWidgetName(std::wstring & name)
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

}

void UIManager::FrameDataSubmit()
{
    // 여기서 서브밋할 예정
    auto* renderer = YunoEngine::GetRenderer();
    renderer->GetCamera().SetOrthoFlag(m_isOrtho);

    renderer->BindConstantBuffers_Camera(dirData);
}
