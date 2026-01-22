#pragma once
#include "InputEvent.h"

class IInputContext
{
public:
    virtual ~IInputContext() = default;

    // 우선순위 >> 나중에 ui추가됐을 때 입력을 소비할 우선순위 만들예정
    virtual int GetPriority() const = 0;

    // 입력 이벤트 소비
    virtual bool OnInputEvent(InputEvent& evt) = 0;   

    virtual bool Event(InputEvent& evt) = 0;
};
