#pragma once

#include "SceneBase.h"
#include "TitleScene_InputContext.h"

class UserImage;
class SceneChangeButton;

class Title final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "Title"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    TitleScene_InputContext m_uiCtx;
    Widget* m_titleImage = nullptr;
    SceneChangeButton* m_startBtn = nullptr;
};
