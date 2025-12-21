#pragma once

#include "ITime.h"
#include <cstdint>

class YunoTimer final : public ITime
{
public:
    void Initialize() override;
    void Tick() override;

    float  DeltaSeconds() const override { return m_deltaSeconds; }
    float  UnscaledDeltaSeconds() const override { return m_unscaledDeltaSeconds; }
    double TotalSeconds() const override { return m_totalSeconds; }

    uint64_t FrameCount() const override { return m_frameCount; }
    double   FPS() const override { return m_fps; }

    void  SetTimeScale(float scale) override;
    float TimeScale() const override { return m_timeScale; }

    void SetPaused(bool paused) override { m_paused = paused; }
    bool IsPaused() const override { return m_paused; }

    void  SetMaxDeltaSeconds(float maxDt) override { m_maxDt = maxDt; }
    float MaxDeltaSeconds() const override { return m_maxDt; }

private:
    // QPC
    double  m_counterToSeconds = 0.0;
    int64_t m_prevCounter = 0;

    // time state
    float   m_unscaledDeltaSeconds = 0.0f;
    float   m_deltaSeconds = 0.0f;
    double  m_totalSeconds = 0.0;

    bool    m_paused = false;
    float   m_timeScale = 1.0f;
    float   m_maxDt = 0.1f; // 100ms clamp

    // stats
    uint64_t m_frameCount = 0;
    double   m_fps = 0.0;
    double   m_fpsAccumTime = 0.0;
    uint32_t m_fpsAccumFrames = 0;
};
