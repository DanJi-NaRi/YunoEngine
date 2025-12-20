#include "pch.h"
#include "YunoEngine.h"
#include "IGameApp.h"   

YunoEngine::YunoEngine()
{
}

YunoEngine::~YunoEngine()
{
	Shutdown();
}

bool YunoEngine::Initialize(IGameApp* game)
{
    if (!game)
        return false;

    // 게임 초기화
    if (!game->OnInit())
        return false;

    m_running = true;
    m_game = game;

    return true;
}

int YunoEngine::Run()
{
    using clock = std::chrono::steady_clock;
    auto prev = clock::now();

    while (m_running)
    {
        auto now = clock::now();
        std::chrono::duration<float> delta = now - prev;
        prev = now;

        float dt = delta.count();

        // 게임 루프 콜백
        m_game->OnUpdate(dt);
        m_game->OnRender();

        // 너무 빨라지지 않게 (임시)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}

void YunoEngine::Shutdown()
{
    if (m_game)
    {
        m_game->OnShutdown();
        m_game = nullptr;
    }
}

bool YunoEngine::IsRunning() const
{
    return m_running;
}
