#include "pch.h"
#include "YunoTimer.h"



void YunoTimer::Initialize()
{
    LARGE_INTEGER freq{};
    QueryPerformanceFrequency(&freq);
    m_counterToSeconds = 1.0 / static_cast<double>(freq.QuadPart);

    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    m_prevCounter = now.QuadPart;

    m_unscaledDeltaSeconds = 0.0f;
    m_deltaSeconds = 0.0f;
    m_totalSeconds = 0.0;

    m_frameCount = 0;
    m_fps = 0.0;
    m_fpsAccumTime = 0.0;
    m_fpsAccumFrames = 0;
}

void YunoTimer::Tick()
{
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    const int64_t deltaCounter = now.QuadPart - m_prevCounter;
    m_prevCounter = now.QuadPart;

    double dt = static_cast<double>(deltaCounter) * m_counterToSeconds;

    // dt clamp (브레이크/탭아웃/리사이즈 드래그 등 대비)
    if (m_maxDt > 0.0f)
        dt = std::min(dt, static_cast<double>(m_maxDt));

    const float unscaled = static_cast<float>(dt);

    m_unscaledDeltaSeconds = unscaled;      // UI 는 이걸로 업데이트

    if (m_paused)
    {
        m_deltaSeconds = 0.0f;
    }
    else
    {
        m_deltaSeconds = unscaled * m_timeScale;
        m_totalSeconds += static_cast<double>(m_deltaSeconds);
    }

    ++m_frameCount;


    m_fpsAccumTime += dt;
    ++m_fpsAccumFrames;

    if (m_fpsAccumTime >= 1.0)
    {
        m_fps = static_cast<double>(m_fpsAccumFrames) / m_fpsAccumTime;
        m_fpsAccumTime = 0.0;
        m_fpsAccumFrames = 0;
    }
}

void YunoTimer::SetTimeScale(float scale)
{
    // 음수 방지
    m_timeScale = std::max(0.0f, scale);
}
