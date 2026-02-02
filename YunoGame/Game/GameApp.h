#pragma once

#include "IGameApp.h"
#include "IAudioManager.h"
#include "YunoClientNetwork.h"
#include "GameManager.h"

class GameApp : public IGameApp
{
public:
    GameApp() = default;
    ~GameApp() override;

    bool OnInit() override;
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float fixedDt) override;
    void OnShutdown() override;


private:
    yuno::game::YunoClientNetwork m_clientNet;
    std::unique_ptr<GameManager> m_gameManager;
};


void CameraMove(float dt);
