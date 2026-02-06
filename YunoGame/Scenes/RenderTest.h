#pragma once

#include "SceneBase.h"

class AnimTest;

class RenderTest final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "RenderTest"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    AnimTest* gun;
    AnimTest* axe;
    AnimTest* scythe;
    AnimTest* blade;
    AnimTest* breacher;
    AnimTest* chakram01;
    AnimTest* chakram02;
};
