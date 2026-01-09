#include "pch.h"

#include "YunoEngine.h"

// 인터페이스
#include "IGameApp.h"
//#include "IWindow.h"
//#include "IRenderer.h"
//#include "ITime.h"
//#include "IInput.h"
//#include "ISceneManager.h"

// 유노
#include "YunoWindow.h"
#include "YunoRenderer.h"
#include "YunoTimer.h"
#include "YunoTextureManager.h"
#include "YunoInputSystem.h"
#include "YunoSceneManager.h"

IRenderer* YunoEngine::s_renderer = nullptr;
ITextureManager* YunoEngine::s_textureManager = nullptr;
IInput* YunoEngine::s_input = nullptr;
IWindow* YunoEngine::s_window = nullptr;
ISceneManager* YunoEngine::s_sceneManager = nullptr;

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

    // 화면 생성
    m_window = std::make_unique<YunoWindow>();          
    if (!m_window->Create(title, width, height))
        return false;
    s_window = m_window.get();

    // 렌더러 생성
    m_renderer = std::make_unique<YunoRenderer>();      
    if (!m_renderer->Initialize(m_window.get()))
        return false;
    s_renderer = m_renderer.get();

    // 씬 매니저 생성
    m_sceneManager = std::make_unique<YunoSceneManager>();
    s_sceneManager = m_sceneManager.get();

    // 인풋 시스템 생성
    m_input = std::make_unique<YunoInputSystem>();
    s_input = m_input.get();

    // 텍스쳐 매니저 생성
    m_textureManager = std::make_unique<YunoTextureManager>(
        static_cast<YunoRenderer*>(m_renderer.get())
    );
    s_textureManager = m_textureManager.get();

    // 타이머 생성
    m_timer = std::make_unique<YunoTimer>();            
    m_timer->Initialize();
    //m_timer->SetMaxDeltaSeconds(0.1f); // 최대 프레임 제한
    m_timer->SetTimeScale(1.0f);
    m_fixedAccumulator = 0.0;

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
        if (m_window->ConsumeResize(w, h))      // 화면 크기 변화 있으면? (더티 플래그 사용)
        {
            m_renderer->Resize(w, h);           // 렌더러 화면도 같이 변경 (스왑 체인, RTV, DSV)
        }


        // ---------------------------------업데이트 시작 -----------------------------------------

        constexpr double fixedDt = 1.0 / 60.0;   // 60Hz
        constexpr int maxFixedStepsPerFrame = 5; 


        // dt 계산
        m_timer->Tick();
        const double frameDt = static_cast<double>(m_timer->UnscaledDeltaSeconds());

        m_fixedAccumulator += frameDt;

        int steps = 0;
        while (m_fixedAccumulator >= fixedDt && steps < maxFixedStepsPerFrame)
        {
            m_game->OnFixedUpdate(static_cast<float>(fixedDt));
            m_fixedAccumulator -= fixedDt;
            ++steps;
        }

        if (steps == maxFixedStepsPerFrame)
        {
            m_fixedAccumulator = 0.0;
        }


        m_input->BeginFrame();
        m_input->Dispatch();


        const float dt = m_timer->DeltaSeconds();
        m_game->OnUpdate(dt);

        // 씬 업데이트 (씬 전환 ApplyPending 포함)
        if (m_sceneManager)
            m_sceneManager->Update(dt);

        // ---------------------------------드로우 시작 -----------------------------------------

        m_renderer->BeginFrame();

        if (m_sceneManager)
            m_sceneManager->Submit(m_renderer.get());

        m_renderer->Flush();

        m_renderer->EndFrame();
    }

    Shutdown();
    return 0;
}

void YunoEngine::Shutdown()
{
    // 종료 순서
    // 1. 게임
    if (m_game)
    {
        m_game->OnShutdown();
        m_game = nullptr;
    }
    // 2. 씬매니저
    s_sceneManager = nullptr;
    m_sceneManager.reset();

    // 3. 텍스쳐 매니저
    s_textureManager = nullptr;
    m_textureManager.reset();

    // 4. 렌더러
    if (m_renderer)
    {
        m_renderer->Shutdown();
        s_renderer = nullptr;
        m_renderer.reset();
    }

    // 5. 타이머 
    m_timer.reset();

    // 6. 윈도우
    s_window = nullptr;
    m_window.reset();

    m_running = false;
}

