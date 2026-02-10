#pragma once

#include "SceneBase.h"
#include "EscScene_inputContext.h"

class SceneChangeButton;
class OptionButton;
class PopButton;
class CheckPanel;

class EscScene final : public SceneBase
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
    void CreateMainUI();
    void CreateCheckUI();

private:
    // Button
    OptionButton* m_leaveBtn = nullptr;
    OptionButton* m_guideBtn = nullptr;
    OptionButton* m_optionBtn = nullptr;
    OptionButton* m_surrenderBtn = nullptr;
    PopButton* m_backBtn = nullptr;
    
    CheckPanel* m_checkPanel = nullptr;

private:
    EscScene_InputContext m_uiCtx;

    float ClientW = 0.f;
    float ClientH = 0.f;

    float btnW = (1538.f / 2) * 0.8;
    float btnH = (105.f / 2) * 0.8;
};
