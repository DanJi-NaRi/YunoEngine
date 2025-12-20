#pragma once
#include "IGameApp.h"  

class GameApp : public IGameApp
{
public:
    bool OnInit() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void OnShutdown() override;
};
