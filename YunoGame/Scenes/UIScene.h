#pragma once

#include "SceneBase.h"

class UIScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void Submit() override;

    const char* GetDebugName() const override { return "UIScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:

};
