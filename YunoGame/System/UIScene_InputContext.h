#pragma once
#include <iostream>

#include <InputEvent.h>
#include "InputContextBase.h"

#include "AudioQueue.h"
#include "UIManager.h"
#include "UIScene.h"
#include "Button.h"


class UIScene_InputContext final : public InputContextBase
{
public:
    int GetPriority() const override { return 100; } // UI 우선

    bool OnInputEvent(InputEvent& evt) override
    {
        //std::cout << "[UI] Event type=" << (int)evt.type << " key=" << evt.key << "\n";

        if (evt.type == InputEventType::MouseButtonDown && evt.key == 0)
        {
            // 씬에서 가지고 있는 Button Rect 비교
            // 있으면 기-> Button Event()
            // 없으면 -> 컨슘 ㅋㅕ
            evt.consumed = m_uiManager->ProcessButtonCursor(ButtonState::Pressed);

            AudioQ::Insert({ AudioCmdType::PlayOneShot, EventName::UI_Click} ); // 사운드!
            std::cout << "[UI] Consumed LMB\n";
            return evt.consumed;
        }
        return false;
    }

    bool Event(InputEvent& evt) override
    {
        return false;
    }

};



