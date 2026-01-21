#pragma once
#include <iostream>

#include <IInputContext.h>
#include <InputEvent.h>

#include "AudioQueue.h"

class Game_InputContext final : public IInputContext
{
public:
    int GetPriority() const override { return 0; } // 게임은 아래

    bool OnInputEvent(InputEvent& evt) override
    {
        //std::cout << "[Game] Event type=" << (int)evt.type << " key=" << evt.key << "\n";
        if (evt.type == InputEventType::MouseButtonDown && evt.key == 0)
        {
            std::cout << "[GAME] Consumed LMB\n";
            evt.consumed = true;
            return true;
        }
        return false;
    }
};
