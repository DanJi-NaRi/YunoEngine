#pragma once

#include "SceneBase.h"
#include "GuideScene_InputContext.h"

class SceneChangeButton;
class OptionButton;
class PopButton;

enum class GuideUIState
{
    Main,
    Keyword,
    CardType,
    Turn,
    CoinToss,
    Registration
};

class GuideScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "GuideScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    // UI 생성
    void CreateMainUI();
    void CreateKeywordUI();
    void CreateCardTypeUI();
    void CreateTurnUI();
    void CreateCoinTossUI();
    void CreateRegistrationUI();

    // 상태
    void ChangeUIState(GuideUIState state);
    void ClearSelection();
    void ClearRoot();
private:
    GuideScene_InputContext m_uiCtx;
    GuideUIState m_uiState = GuideUIState::Main;

    float ClientW = 0.f;
    float ClientH = 0.f;
       
    // Button
    OptionButton* m_keywordBtn = nullptr;
    OptionButton* m_cardTypeBtn = nullptr;
    OptionButton* m_turnBtn = nullptr;
    OptionButton* m_coinTossBtn = nullptr;
    OptionButton* m_registrationBtn = nullptr;

    // Root
    Widget* m_keywordRoot = nullptr;
    Widget* m_cardTypeRoot = nullptr;
    Widget* m_turnRoot = nullptr;
    Widget* m_coinTossRoot = nullptr;
    Widget* m_registrationRoot = nullptr;

    // Back
    PopButton* m_backBtn = nullptr;
};
