#include "pch.h"
#include "YunoInputSystem.h"

#include "InputContextBase.h"
#include "SceneBase.h"


void YunoInputSystem::BeginFrame()
{
    m_state.BeginFrame();
}

void YunoInputSystem::PushEvent(const InputEvent& evt)
{
    m_events.push_back(evt);

    ApplyToState(evt);
}

bool YunoInputSystem::IsKeyDown(uint32_t key) const
{
    if (key >= m_state.keyDown.size()) return false;
    return m_state.keyDown[key] != 0;
}

bool YunoInputSystem::IsKeyPressed(uint32_t key) const
{
    if (key >= m_state.keyDown.size()) return false;
    return (m_state.keyDown[key] != 0) && (m_state.prevKeyDown[key] == 0);
}

bool YunoInputSystem::IsKeyReleased(uint32_t key) const
{
    if (key >= m_state.keyDown.size()) return false;
    return (m_state.keyDown[key] == 0) && (m_state.prevKeyDown[key] != 0);
}

bool YunoInputSystem::IsMouseHovered() const
{
    return m_state.mouseHovered;
}

bool YunoInputSystem::IsMouseButtonDown(uint32_t button) const
{
    if (button >= m_state.mouseDown.size()) return false;
    return m_state.mouseDown[button] != 0;
}

bool YunoInputSystem::IsMouseButtonPressed(uint32_t button) const
{
    if (button >= m_state.mouseDown.size()) return false;
    return (m_state.mouseDown[button] != 0) && (m_state.prevMouseDown[button] == 0);
}

bool YunoInputSystem::IsMouseButtonReleased(uint32_t button) const
{
    if (button >= m_state.mouseDown.size()) return false;
    return (m_state.mouseDown[button] == 0) && (m_state.prevMouseDown[button] != 0);
}

void YunoInputSystem::AddContext(IInputContext* context, IScene* scene)
{
    if (!context || !scene) return;
    
    InputContextBase* pContext = dynamic_cast<InputContextBase*>(context);

    if (!pContext) return;

    auto it = std::find(m_contexts.begin(), m_contexts.end(), context);
    if (it != m_contexts.end())
        return;

    pContext->SetScene(scene); // 씬 할당
    m_contexts.push_back(context);
    m_contextOrderDirty = true;
}

void YunoInputSystem::RemoveContext(IInputContext* context)
{
    InputContextBase* pContext = dynamic_cast<InputContextBase*>(context);
    pContext->Clear();
    auto it = std::remove(m_contexts.begin(), m_contexts.end(), context);
    if (it != m_contexts.end())
    {
        m_contexts.erase(it, m_contexts.end());
    }
}

void YunoInputSystem::MouseTrack(HWND hWnd, BOOL bOn)
{
    DWORD flag = bOn ? TME_HOVER : TME_CANCEL;
    DWORD time = bOn ? 100 : 0;

    TRACKMOUSEEVENT ev = {};
    //ZeroMemory(&ev, sizeof(ev));
    ev.cbSize = sizeof(TRACKMOUSEEVENT);
    ev.dwFlags = flag;
    ev.hwndTrack = hWnd;
    ev.dwHoverTime = time;
    TrackMouseEvent(&ev);
}

void YunoInputSystem::SetInputBlockScene(IScene* scene)
{
    m_blockBelowScene = scene;
}

void YunoInputSystem::SortContextsIfDirty()
{
    if (!m_contextOrderDirty) return;

    // 내림차순 정렬 우선순위가 크면 먼저 받음
    std::sort(m_contexts.begin(), m_contexts.end(),
        [](IInputContext* a, IInputContext* b)
        {
            const int pa = a ? a->GetPriority() : -999999;
            const int pb = b ? b->GetPriority() : -999999;
            return pa > pb;
        });

    m_contextOrderDirty = false;
}

void YunoInputSystem::Dispatch()
{
    SortContextsIfDirty();

    while (!m_events.empty())
    {
        InputEvent evt = m_events.front();
        m_events.pop_front();

        // Dispatch 시작 시 차단 기준 고정
        IScene* blockScene = m_blockBelowScene;

        for (IInputContext* ctx : m_contexts)
        {
            if (!ctx) continue;

            auto* base = static_cast<InputContextBase*>(ctx);
            IScene* ownerScene = base->GetScene();

            // 차단 기준 아래는 절대 전달 안 함
            if (blockScene && ownerScene != blockScene)
                continue;

            ctx->OnInputEvent(evt);

            if (evt.consumed)
                break;
        }
    }
}

void YunoInputSystem::ApplyToState(const InputEvent& evt)
{
    switch (evt.type)
    {
    case InputEventType::KeyDown:
        if (evt.key < m_state.keyDown.size())
            m_state.keyDown[evt.key] = 1;
        break;

    case InputEventType::KeyUp:
        if (evt.key < m_state.keyDown.size())
            m_state.keyDown[evt.key] = 0;
        break;

    case InputEventType::MouseButtonDown:
        if (evt.key < m_state.mouseDown.size())
            m_state.mouseDown[evt.key] = 1;

        // 클릭 시작했을 때 마우스 좌표
        m_state.pressedMouseX = evt.x;
        m_state.pressedMouseY = evt.y;
        break;

    case InputEventType::MouseButtonUp:
        if (evt.key < m_state.mouseDown.size())
            m_state.mouseDown[evt.key] = 0;

        m_state.pressedMouseX = 0.0f;
        m_state.pressedMouseY = 0.0f;
        break;

    case InputEventType::MouseMove:
    {
        // delta 계산: 이전 좌표 기준
        const float prevX = m_state.mouseX;
        const float prevY = m_state.mouseY;

        m_state.mouseX = evt.x;
        m_state.mouseY = evt.y;

        m_state.mouseDeltaX += (m_state.mouseX - prevX);
        m_state.mouseDeltaY += (m_state.mouseY - prevY);

        m_state.mouseHovered = false;
        break;
    }
    
    case InputEventType::MouseHover:
        m_state.mouseHovered = true;
        break;

    case InputEventType::MouseWheel:
        // 상태 기반 wheel 누적이 필요하면 InputState에 wheelDelta 추가해서 누적
        break;

    default:
        break;
    }
}
