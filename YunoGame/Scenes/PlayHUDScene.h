#pragma once

#include "SceneBase.h"

class PlayHUDScene final : public SceneBase
{
public:

    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "PlayHUDScene"; };

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    Widget* m_pTurn = nullptr;    // 1의 자리수
    Widget* m_pTurn10 = nullptr;  // 10의 자리수
};
