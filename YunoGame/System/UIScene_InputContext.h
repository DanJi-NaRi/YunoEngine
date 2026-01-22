#pragma once
#include <iostream>

#include "YunoEngine.h"

#include "InputEvent.h"
#include "InputContextBase.h"
#include "IInput.h"

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
        auto input = YunoEngine::GetInput();

        bool LMB_Pressed = (evt.type == InputEventType::MouseButtonDown 
            && evt.key == 0 && input->IsMouseButtonPressed(0));

        bool RMB_Pressed = (evt.type == InputEventType::MouseButtonDown 
            && evt.key == 1 && input->IsMouseButtonPressed(1));

        bool LMB_Up =      (evt.type == InputEventType::MouseButtonUp   
            && evt.key == 0 && input->IsMouseButtonReleased(0));

        bool RMB_Up = (evt.type == InputEventType::MouseButtonUp
            && evt.key == 1 && input->IsMouseButtonReleased(1));

        bool KeyPressed =  (evt.type == InputEventType::KeyDown         
            && evt.key != 0 && input->IsKeyPressed(evt.key));

        bool KeyUp =       (evt.type == InputEventType::KeyUp           
            && evt.key != 0 && input->IsKeyReleased(evt.key));

        if (LMB_Pressed)
        {
            // 마우스 먼저
            evt.consumed = m_uiManager->ProcessButtonMouse(ButtonState::Pressed, 0);
            if (evt.consumed) {
                
                std::cout << "[UI] Consumed LMB\n";
            }
            return evt.consumed;
        }
        else if (RMB_Pressed) { 
            evt.consumed = m_uiManager->ProcessButtonMouse(ButtonState::Pressed, 1);     
            if(evt.consumed) std::cout << "[UI] Consumed RMB\n";
            return evt.consumed;
        }
        else if (LMB_Up)      { return evt.consumed = m_uiManager->ProcessButtonMouse(ButtonState::Released, 0);    }
        else if (RMB_Up)      { return evt.consumed = m_uiManager->ProcessButtonMouse(ButtonState::Released, 1);    }
        else if (KeyPressed)  { return evt.consumed = m_uiManager->ProcessButtonKey(ButtonState::Pressed, evt.key); }
        else if (KeyUp) { return evt.consumed = m_uiManager->ProcessButtonKey(ButtonState::Released, evt.key);      };
        return false;
    }

    bool Event(InputEvent& evt) override
    {
        return false;
    }

};



