#pragma once
#include "ISceneManager.h"
#include "IScene.h"

class YunoSceneManager final : public ISceneManager
{
public:
    YunoSceneManager() = default;
    ~YunoSceneManager() override;

    void RequestSetActive(std::unique_ptr<IScene> next) override;
    void Update(float dt) override;
    void Submit(IRenderer* renderer) override;

    IScene* GetActive() const override { return m_active.get(); }

private:
    void ApplyPendingScene();

private:
    ScenePtr m_active;  // 현재 씬
    ScenePtr m_pending; // 변경할 예정인 씬
};
