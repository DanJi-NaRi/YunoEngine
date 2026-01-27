#pragma once

#include "SceneBase.h"

class UserImage;

class Title final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "Title"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    Widget* m_titleImage = nullptr;
};
