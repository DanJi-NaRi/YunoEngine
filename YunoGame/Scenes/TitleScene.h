#pragma once

#include "SceneBase.h"


class TitleScene final : public SceneBase
{
public:
    void OnEnter() override;
    void OnExit() override;

    void Update(float dt) override;
    void Submit() override;

    const char* GetDebugName() const override { return m_name.c_str(); }

protected:
    bool OnCreateScene() override;
    void OnDestroyScene() override;

private:
    //Quad* m_plane = nullptr;   // 간단하게 raw로 들고, 오브젝트매니저가 delete
    //Building* m_building = nullptr;   // 간단하게 raw로 들고, 오브젝트매니저가 delete
};
