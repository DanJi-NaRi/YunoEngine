#pragma once

#include "SceneBase.h"

class SceneTemplate final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "SampleScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    
};
