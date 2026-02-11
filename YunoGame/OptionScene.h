#pragma once

#include "SceneBase.h"
#include "OptionScene_InputContext.h"

class SceneChangeButton;
class OptionButton;
class PopButton;
class VolumePanel;

enum class OptionUIState
{
    Main,
    Volume,
    Credit
};

class OptionScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "OptionScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    // UI 생성
    void CreateMainUI();
    void CreateVolumeUI();
    void CreateCreditUI();

    // 상태
    void ChangeUIState(OptionUIState state);
private:
    OptionScene_InputContext m_uiCtx;

    OptionUIState m_uiState = OptionUIState::Main;

    float ClientW = 0.f;
    float ClientH = 0.f;

    // Main
    OptionButton* m_volumeBtn = nullptr;
    OptionButton* m_creditBtn = nullptr;

    // Volume
    std::array<VolumePanel*, 3> m_volumePanels{ nullptr, nullptr, nullptr };
    //std::array<Widget*, 1> m_volumePanels{ nullptr};
    Widget* m_volumeRoot = nullptr;

    // Credit
    Widget* m_creditRoot = nullptr;

    // Back
    PopButton* m_backBtn = nullptr;
};
