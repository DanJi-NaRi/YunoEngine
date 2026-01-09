#include "pch.h"
#include "YunoInputSystem.h"

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

bool YunoInputSystem::IsMouseButtonDown(uint32_t button) const
{
    if (button >= m_state.mouseDown.size()) return false;
    return m_state.mouseDown[button] != 0;
}

void YunoInputSystem::AddContext(IInputContext* context)
{
    if (!context) return;

    auto it = std::find(m_contexts.begin(), m_contexts.end(), context);
    if (it != m_contexts.end())
        return;

    m_contexts.push_back(context);
    m_contextOrderDirty = true;
}

void YunoInputSystem::RemoveContext(IInputContext* context)
{
    auto it = std::remove(m_contexts.begin(), m_contexts.end(), context);
    if (it != m_contexts.end())
    {
        m_contexts.erase(it, m_contexts.end());
    }
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

        // 컨텍스트 우선순위 순으로 전달
        for (IInputContext* ctx : m_contexts)
        {
            if (!ctx) continue;

            const bool consumed = ctx->OnInputEvent(evt);
            if (consumed || evt.consumed)
            {
                // 상위 컨텍스트에서 이벤트 소모되면 하위 컨텍스트로 안내려감
                // 이거 조금 바꾸면 모든 레이어에서도 소모 가능하긴 함
                // 특정 레이어에서만 소모하게 하는거도 컨텍스트 바꾸면 되긴할듯?
                break;
            }
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
        break;

    case InputEventType::MouseButtonUp:
        if (evt.key < m_state.mouseDown.size())
            m_state.mouseDown[evt.key] = 0;
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
        break;
    }

    case InputEventType::MouseWheel:
        // 상태 기반 wheel 누적이 필요하면 InputState에 wheelDelta 추가해서 누적
        break;

    default:
        break;
    }
}
