#pragma once

#include "SceneBase.h"

#include "WeaponSelectScene_InputContext.h"

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

private:

    //std::array<Widget*, 4> m_pUserImages;
    Widget* m_pUserImage0 = nullptr;
    Widget* m_pUserImage1 = nullptr;
    Widget* m_pUserImage2 = nullptr;
    Widget* m_pUserImage3 = nullptr;

    //std::array<Widget*, 6> m_pWeaponBtns;
    Widget* m_PWeaponBtn_Blaster    = nullptr;
    Widget* m_PWeaponBtn_Breacher   = nullptr;
    Widget* m_PWeaponBtn_Impactor   = nullptr;
    Widget* m_PWeaponBtn_Chakram    = nullptr;
    Widget* m_PWeaponBtn_Scythe     = nullptr;
    Widget* m_PWeaponBtn_Cleaver    = nullptr;

    Widget* m_pReadyBtn = nullptr;
    Widget* m_pExitBtn  = nullptr;

    WeaponSelectScene_InputContext m_weaponCtx;
};
