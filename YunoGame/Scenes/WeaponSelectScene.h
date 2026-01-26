#pragma once

#include "SceneBase.h"

class Test_Unit;

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

    Test_Unit* unit1 = nullptr;
    Test_Unit* unit2 = nullptr;
    Test_Unit* unit3 = nullptr;
    Test_Unit* unit4 = nullptr;
};
