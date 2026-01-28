#pragma once

#include "SceneBase.h"
#include "Game_InputContext.h"
#include "IGridSystem.h"
//class Game_InputContext;
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
    void TestInput();

private:
    std::unique_ptr<IGridSystem> m_playGrid;
    Game_InputContext m_gameCtx;
};
