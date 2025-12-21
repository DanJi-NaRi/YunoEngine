#include "pch.h"

#include "YunoEngine.h"

// 인터페이스
#include "IGameApp.h"
#include "IWindow.h"

// 유노
#include "YunoWindow.h"
#include "YunoRenderer.h"

YunoEngine::YunoEngine() = default;
YunoEngine::~YunoEngine()
{
    Shutdown();
}

bool YunoEngine::Initialize(IGameApp* game, const wchar_t* title, uint32_t width, uint32_t height)
{
    if (!game)
        return false;

    m_game = game;

    m_window = std::make_unique<YunoWindow>();          // 화면 생성
    if (!m_window->Create(title, width, height))
        return false;

    m_renderer = std::make_unique<YunoRenderer>();      // 렌더러 생성
    if (!m_renderer->Initialize(m_window.get()))
        return false;

    // Timer 초기화
    InitTimer();

    // Game 초기화
    if (!m_game->OnInit())
        return false;

    m_running = true;
    return true;
}

int YunoEngine::Run()
{
    if (!m_running || !m_window || !m_game)
        return -1;

    while (m_running)
    {
        m_window->PollEvents(); // OS한테 메시지 전달

        if (m_window->ShouldClose())    // 종료
        {
            m_running = false;
            break;
        }

        uint32_t w = 0, h = 0;
        if (m_window->ConsumeResize(w, h))      // 화면 크기 변화 있으면?
        {
            m_renderer->Resize(w, h);           // 렌더러 화면도 같이 변경 (스왑 체인, RTV, DSV)
        }

        // dt 계산
        const float dt = TickDeltaSeconds();

        m_renderer->BeginFrame();
        m_game->OnUpdate(dt);
        m_renderer->EndFrame();
    }

    Shutdown();
    return 0;
}

void YunoEngine::Shutdown()
{
    // 1) 게임 종료 콜백 (게임이 렌더 리소스를 들고 있을 수도 있음)
    if (m_game)
    {
        m_game->OnShutdown();
        m_game = nullptr;
    }

    // 2) 렌더러 먼저 종료 (D3D 리소스 Release, 디버그 리포트도 여기서)
    if (m_renderer)
    {
        m_renderer->Shutdown();
        m_renderer.reset();
    }

    // 3) 그 다음 윈도우 종료
    m_window.reset();

    m_running = false;
}

void YunoEngine::InitTimer()
{
    LARGE_INTEGER freq{};
    QueryPerformanceFrequency(&freq);
    m_counterToSeconds = 1.0 / static_cast<double>(freq.QuadPart);

    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    m_prevCounter = now.QuadPart;
}

float YunoEngine::TickDeltaSeconds()
{
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    const long long delta = now.QuadPart - m_prevCounter;
    m_prevCounter = now.QuadPart;

    const double dt = static_cast<double>(delta) * m_counterToSeconds;
    return static_cast<float>(dt);
}
