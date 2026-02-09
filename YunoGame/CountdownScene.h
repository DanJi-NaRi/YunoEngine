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
    void ShowOrUpdateCountdownImage(int num);

    UINT m_countdownWidgetID = UINT_MAX;
    int  m_lastNumber = -1;
};
