#pragma once
#include <IInputContext.h>

class GameInputContext : public IInputContext
{
public:
    int GetPriority() const override { return 0; } // UI보다 낮게
    bool OnInputEvent(InputEvent& evt) override;
};
