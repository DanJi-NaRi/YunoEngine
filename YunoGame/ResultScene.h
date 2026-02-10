#pragma once

#include "SceneBase.h"
#include "ResultScene_inputContext.h"

class SceneChangeButton;
class OptionButton;
class ResultPanel;

enum class ResultUIState
{
    Main,
    Draw,
    Victory,
    Defeat,
};


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

    void CreateUI();
    void ChangeUIState(ResultUIState state);

private:
    ResultScene_InputContext m_uiCtx;
    ResultUIState m_uiState = ResultUIState::Main;

    ResultPanel* m_victoryPanel = nullptr;
    ResultPanel* m_defeatPanel = nullptr;
    ResultPanel* m_drawPanel = nullptr;

    OptionButton* m_titleBtn = nullptr;

    float ClientW = 0.f;
    float ClientH = 0.f;

    int winner = 0;
    int m_pID = 0;
};
