#pragma once

#include "SceneBase.h"

#include "WeaponSelectScene_InputContext.h"

class CursurSystem;
class SceneChangeButton;
class WeaponButton;
class ReadyButton;
class ShowCardDeck;

class WeaponSelectScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "WeaponSelectScene"; }


protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

    void HandleCountdownUI(float dt);
    void ApplyCountDownUnits();
    void ApplyOpponentPresenceVisuals();
    void ApplyReadyStateVisuals();
private:

    //std::array<Widget*, 4> m_pUserImages;
    Widget* m_pUserImage0 = nullptr; // 왼쪽
    Widget* m_pUserImage1 = nullptr;
    Widget* m_pUserImage2 = nullptr; // 오른쪽
    Widget* m_pUserImage3 = nullptr;

    Widget* m_pWeaponImage0 = nullptr;
    Widget* m_pWeaponImage1 = nullptr;
    Widget* m_pWeaponImage2 = nullptr;
    Widget* m_pWeaponImage3 = nullptr;

    Widget* m_pWeaponNameImage = nullptr;
    Widget* m_pWeaponCardImage = nullptr;

    ShowCardDeck* m_showCardDeck = nullptr;

    WeaponButton* m_PWeaponBtn_Blaster    = nullptr;
    WeaponButton* m_PWeaponBtn_Breacher   = nullptr;
    WeaponButton* m_PWeaponBtn_Impactor   = nullptr;
    WeaponButton* m_PWeaponBtn_Chakram    = nullptr;
    WeaponButton* m_PWeaponBtn_Scythe     = nullptr;
    WeaponButton* m_PWeaponBtn_Cleaver    = nullptr;

    ReadyButton* m_pReadyBtn = nullptr;



    UINT m_countdownWidgetID = 0;
    int  m_lastCountdownNumber = -1;


    SceneChangeButton* m_pExitBtn  = nullptr;

    WeaponSelectScene_InputContext m_weaponCtx;
    CursurSystem* m_pCursurSys = nullptr;
};
