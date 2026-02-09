#pragma once

#include "SceneBase.h"

class StandByScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "StandByScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    Widget* m_pStandByBackImage = nullptr;
    Widget* m_pStandByImage = nullptr;
};
