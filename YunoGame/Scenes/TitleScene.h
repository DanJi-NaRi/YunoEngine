#pragma once
#include <iostream>

#include "IScene.h"

class Quad;

class TitleScene final : public IScene
{
public:
    bool OnCreate() override;
    void OnEnter() override { std::cout << "[TitleScene] OnEnter\n"; }
    void OnExit() override { std::cout << "[TitleScene] OnExit\n"; }
    void OnDestroy() override;

    void Update(float dt) override;
    void Submit(IRenderer* renderer) override;

private:
    Quad* m_plane = nullptr;   // 간단하게 raw로 들고, OnDestroy에서 delete
    float m_lastDt = 0.0f;
};
