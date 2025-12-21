#include "pch.h"
#include "GameApp.h"

#include <iostream>

bool GameApp::OnInit()
{
    std::cout << "[GameApp] OnInit\n";
    return true;
}

void GameApp::OnUpdate(float dt)
{
    static float acc = 0.0f;
    acc += dt;
    if (acc >= 1.0f)
    {
        std::cout << "[GameApp] dt = " << dt << "\n";
        acc = 0.0f;
    }
}

void GameApp::OnFixedUpdate(float fixedDt)
{
    static int step = 0;
    ++step;

    if (step % 60 == 0)
    {
        std::cout << "[GameApp] FixedUpdate dt = " << fixedDt << "\n";
    }
}

void GameApp::OnShutdown()
{
    std::cout << "[GameApp] OnShutdown\n";
}
