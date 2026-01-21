#pragma once

#include "IGameApp.h"

#include "Quad.h"
#include "Triangle.h"

class IAudioManager;

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
    std::unique_ptr<Quad> m_quad;
    std::unique_ptr<Triangle> m_triangle;

    //std::unique_ptr<IAudioManager> m_AM;
    //IAudioManager* m_AM;
};


void CameraMove(float dt);
