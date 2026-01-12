#pragma once


class IGameApp;
class IWindow;
class IRenderer;
class ITime;
class ITextureManager;
class IWindow;
class IInput;
class YunoInputSystem;
class ISceneManager;


class YunoEngine
{
public:
    YunoEngine();
    ~YunoEngine();

    bool Initialize(IGameApp* game, const wchar_t* title, uint32_t width, uint32_t height);
    int Run();
    void Shutdown();

    bool IsRunning() const { return m_running; }

    static IRenderer* GetRenderer() { return s_renderer; }
    static ITextureManager* GetTextureManager() { return s_textureManager; }
    static IInput* GetInput() { return s_input; }
    static IWindow* GetWindow() { return s_window; }
    static ISceneManager* GetSceneManager() { return s_sceneManager; }


private:
    bool m_running = false;                             // 엔진 작동 여부
    IGameApp* m_game = nullptr;                         // 엔진으로 돌릴 게임
    std::unique_ptr<IWindow>    m_window;               // 화면
    std::unique_ptr<IRenderer>  m_renderer;             // 렌더러
    std::unique_ptr<ITime>      m_timer;                // 타이머
    std::unique_ptr<ITextureManager> m_textureManager;  // 텍스쳐 매니저
    std::unique_ptr<YunoInputSystem> m_input;           // 인풋 시스템
    std::unique_ptr<ISceneManager> m_sceneManager;      // 씬 매니저
    double  m_fixedAccumulator = 0.0;                   // FixedUpdate 누적 시간

    static IRenderer* s_renderer;
    static ITextureManager* s_textureManager;
    static IInput* s_input;
    static IWindow* s_window;
    static ISceneManager* s_sceneManager;
};
