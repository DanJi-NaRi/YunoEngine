#pragma once

#include "SceneBase.h"
#include "OptionScene_InputContext.h"
#include "OptionButton.h"
#include "VolumeTrackButton.h"

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

    // 볼륨
    void SetVolume(int level);
    void UpdateVolumeByMouse(int mouseX);

private:
    OptionScene_InputContext m_uiCtx;

    OptionUIState m_uiState = OptionUIState::Main;

    float ClientW = 0.f;
    float ClientH = 0.f;

    // Main
    OptionButton* m_volumeBtn = nullptr;
    OptionButton* m_creditBtn = nullptr;

    // Volume
    Widget* m_volumeRoot = nullptr;
    VolumeTrackButton* m_volumeTrack = nullptr;
    std::vector<Widget*> m_volumeSteps;
    int m_volumeLevel = 10;

    // Credit
    Widget* m_creditRoot = nullptr;

    // Common
    OptionButton* m_backBtn = nullptr;
};
