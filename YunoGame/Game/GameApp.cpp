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
     std::cout << "[GameApp] dt = " << dt << "\n";
}

void GameApp::OnShutdown()
{
    std::cout << "[GameApp] OnShutdown\n";
}
