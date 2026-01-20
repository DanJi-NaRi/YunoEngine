#pragma once

#include "SceneBase.h"

class Triangle;

class PlayScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return m_name.c_str(); }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:

};
