#pragma once

#include "SceneBase.h"

class CountdownScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "CountdownScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    enum class VsPhase : uint8_t
    {
        Countdown,
        VsSlide,
        Finished
    };


    void ShowOrUpdateCountdownImage(int num);
    void StartVsSlide();
    void UpdateVsSlide(float dt);
    void ClearVsWidgets();



    UINT m_countdownWidgetID = UINT_MAX;
    UINT m_leftVsWidgetID = UINT_MAX;
    UINT m_rightVsWidgetID = UINT_MAX;

    int  m_lastNumber = -1;

    float m_vsElapsed = 0.0f;
    VsPhase m_phase = VsPhase::Countdown;
    bool m_roundStartRequested = false;
};
