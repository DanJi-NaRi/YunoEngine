#pragma once
#include "Interface/IGameApp.h"

class GameApp : public IGameApp
{
public:
    bool OnInit() override;
    void OnUpdate(float dt) override;
    void OnShutdown() override;
};
