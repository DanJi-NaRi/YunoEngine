#pragma once
#include <iostream>

#include "IScene.h"

class Triangle;

class PlayScene final : public IScene
{
public:
    bool OnCreate() override;
    void OnEnter() override { std::cout << "[PlayScene] OnEnter\n"; }
    void OnExit() override { std::cout << "[PlayScene] OnExit\n"; }
    void OnDestroy() override;

    void Update(float dt) override;
    void Submit(IRenderer* renderer) override;

private:
    Triangle* m_player = nullptr;
    float m_lastDt = 0.0f;
};
