#pragma once

#include "SceneBase.h"

#include "WeaponSelectScene_InputContext.h"

class CursurSystem;
class SceneChangeButton;

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
private:

    //std::array<Widget*, 4> m_pUserImages;
    Widget* m_pUserImage0 = nullptr; // 왼쪽
    Widget* m_pUserImage1 = nullptr;
    Widget* m_pUserImage2 = nullptr; // 오른쪽
    Widget* m_pUserImage3 = nullptr;

    //std::array<Widget*, 6> m_pWeaponBtns;
    Widget* m_PWeaponBtn_Blaster    = nullptr;
    Widget* m_PWeaponBtn_Breacher   = nullptr;
    Widget* m_PWeaponBtn_Impactor   = nullptr;
    Widget* m_PWeaponBtn_Chakram    = nullptr;
    Widget* m_PWeaponBtn_Scythe     = nullptr;
    Widget* m_PWeaponBtn_Cleaver    = nullptr;

    UINT m_countdownWidgetID = 0;
    int  m_lastCountdownNumber = -1;

    Widget* m_pReadyBtn = nullptr;
    SceneChangeButton* m_pExitBtn  = nullptr;

    WeaponSelectScene_InputContext m_weaponCtx;
    CursurSystem* m_pCursurSys = nullptr;
};
