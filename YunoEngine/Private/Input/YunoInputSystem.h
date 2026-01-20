#pragma once
#include <IInput.h>        
#include <InputEvent.h>    
#include <IInputContext.h> 

#include "InputState.h" 

class YunoInputSystem final : public IInput
{
public:

    void BeginFrame();   // 델타 초기화 등
    void Dispatch();     // 이벤트 큐 >> 컨텍스트로 전달

public:
    // 입력 이벤트 큐에 삽입
    void PushEvent(const InputEvent& evt) override;

    // 게임에서 사용할 API함수들
    bool IsKeyDown(uint32_t key) const override;
    bool IsKeyPressed(uint32_t key) const override;
    bool IsKeyReleased(uint32_t key) const override;

    bool IsMouseButtonDown(uint32_t button) const override;
    bool IsMouseButtonPressed(uint32_t button) const override;
    bool IsMouseButtonReleased(uint32_t button) const override;

    float GetMouseX() const override { return m_state.mouseX; }
    float GetMouseY() const override { return m_state.mouseY; }
    float GetMouseDeltaX() const override { return m_state.mouseDeltaX; }
    float GetMouseDeltaY() const override { return m_state.mouseDeltaY; }

    // 인풋을 사용할 컨텍스트
    void AddContext(IInputContext* context) override;
    void RemoveContext(IInputContext* context) override;

private:
    void SortContextsIfDirty();
    void ApplyToState(const InputEvent& evt);

private:
    InputState m_state{};

    std::deque<InputEvent> m_events;

    std::vector<IInputContext*> m_contexts;
    bool m_contextOrderDirty = false;
};
