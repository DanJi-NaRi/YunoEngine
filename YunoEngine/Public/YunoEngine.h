#pragma once


class IGameApp;
class IWindow;
class IRenderer;
class ITime;

class YunoEngine
{
public:
    YunoEngine();
    ~YunoEngine();

    bool Initialize(IGameApp* game, const wchar_t* title, uint32_t width, uint32_t height);
    int Run();
    void Shutdown();

    bool IsRunning() const { return m_running; }

private:
    bool m_running = false;                  // 엔진 작동 여부
    IGameApp* m_game = nullptr;              // 엔진으로 돌릴 게임
    std::unique_ptr<IWindow>    m_window;    // 화면
    std::unique_ptr<IRenderer>  m_renderer;  // 렌더러
    std::unique_ptr<ITime>      m_timer;     // 타이머
    double  m_fixedAccumulator = 0.0;        // FixedUpdate 누적 시간

};
