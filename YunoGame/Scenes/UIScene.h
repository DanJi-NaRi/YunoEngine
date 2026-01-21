#pragma once

#include "SceneBase.h"

class UIScene final : public SceneBase
{
public:
    UIScene();
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "UIScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:

};
