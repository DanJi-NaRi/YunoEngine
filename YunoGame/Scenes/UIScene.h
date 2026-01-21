#pragma once

#include "SceneBase.h"
#include "UIScene_InputContext.h"
//class UI_InputContext;

class UIScene final : public SceneBase
{
public:

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
    UIScene_InputContext m_uiCtx;
};
