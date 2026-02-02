#pragma once

#include <cstdint>

class ITime
{
public:
    virtual ~ITime() = default;

    // 초기화
    virtual void Initialize() = 0;

    // 매 프레임 호출
    virtual void Tick() = 0;

    // TimeScale 적용 dt (Pause면 0)
    virtual float DeltaSeconds() const = 0;

    // QPC 기반 원본 dt
    virtual float UnscaledDeltaSeconds() const = 0;

    // 누적 시간 DeltaSeconds 누적
    virtual double TotalSeconds() const = 0;

    virtual uint64_t FrameCount() const = 0;
    virtual double FPS() const = 0;

    virtual void SetTimeScale(float scale) = 0;
    virtual float TimeScale() const = 0;

    virtual void SetPaused(bool paused) = 0;
    virtual bool IsPaused() const = 0;

    // dt 폭발 방지 (예: 0.1 = 100ms)
    virtual void SetMaxDeltaSeconds(float maxDt) = 0;
    virtual float MaxDeltaSeconds() const = 0;
};
