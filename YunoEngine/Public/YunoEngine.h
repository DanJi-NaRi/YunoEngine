#pragma once


class IGameApp;


class YunoEngine
{
public:
    YunoEngine();
    ~YunoEngine();

    // 엔진 초기화
    bool Initialize(IGameApp* game);

    // 엔진 실행 (메인 루프)
    int Run();

    // 엔진 종료
    void Shutdown();

    // 실행 상태
    bool IsRunning() const;

private:
    bool m_running = false;
    IGameApp* m_game = nullptr; 
};
