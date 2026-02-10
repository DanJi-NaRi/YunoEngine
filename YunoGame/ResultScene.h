#pragma once

#include "SceneBase.h"
#include "ResultScene_inputContext.h"

class SceneChangeButton;
class OptionButton;
class PopButton;
class CheckPanel;

class ResultScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "EscScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;
private:

private:

private:
    ResultScene_InputContext m_uiCtx;

    float ClientW = 0.f;
    float ClientH = 0.f;

    int winner = 0;
};
