#pragma once

#include "SceneBase.h"
#include "Game_InputContext.h"

class IGridSystem;

class TitleScene final : public SceneBase
{
public:
    TitleScene();
    ~TitleScene();

    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void SubmitObj() override;
    void SubmitUI() override;

    const char* GetDebugName() const override { return "TitleScene"; }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    //Quad* m_plane = nullptr;   // 간단하게 raw로 들고, 오브젝트매니저가 delete
    //Building* m_building = nullptr;   // 간단하게 raw로 들고, 오브젝트매니저가 delete
    Game_InputContext m_gameCtx;
    std::unique_ptr<IGridSystem> m_gridSystem;
};
