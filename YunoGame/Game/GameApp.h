#pragma once

#include "IGameApp.h"

#include "ClientNet.h"

class GameApp : public IGameApp
{
public:
    GameApp() = default;
    ~GameApp() override;

    bool OnInit() override;
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float fixedDt) override;
    void OnShutdown() override;



// 네트워크
private:
    std::unique_ptr<ClientNet> m_net;
    float m_netPingAcc = 0.0f;
};


void CameraMove(float dt);
