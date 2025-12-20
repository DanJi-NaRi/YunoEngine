#include "pch.h"

#include "GameApp.h"


bool GameApp::OnInit()
{
    std::cout << "Game Init\n";
    return true;
}

void GameApp::OnUpdate(float dt)
{
    // dt 확인용
    // 너무 많이 찍히면 콘솔이 폭주하니 필요하면 조건 걸어도 됨
     std::cout << "Update dt=" << dt << "\n";
}

void GameApp::OnRender()
{
    // 렌더 대신 출력
     std::cout << "Render\n";
}

void GameApp::OnShutdown()
{
    std::cout << "Game Shutdown\n";
}
