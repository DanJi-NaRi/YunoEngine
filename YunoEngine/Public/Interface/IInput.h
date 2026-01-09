#pragma once
#include <cstdint>

struct InputEvent;
class IInputContext;

class IInput
{
public:
    virtual ~IInput() = default;

    // 입력 이벤트 큐에 삽입
    virtual void PushEvent(const InputEvent& evt) = 0;

    // 게임에서 사용할 API함수들
    virtual bool IsKeyDown(uint32_t key) const = 0;
    virtual bool IsMouseButtonDown(uint32_t button) const = 0;

    virtual float GetMouseX() const = 0;
    virtual float GetMouseY() const = 0;
    virtual float GetMouseDeltaX() const = 0;
    virtual float GetMouseDeltaY() const = 0;

    // 인풋을 사용할 컨텍스트
    virtual void AddContext(IInputContext* context) = 0;
    virtual void RemoveContext(IInputContext* context) = 0;
};
