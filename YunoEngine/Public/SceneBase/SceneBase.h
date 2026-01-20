#pragma once

#include "IScene.h"

class ObjectManager;

class SceneBase : public IScene
{
public:
    SceneBase();
    virtual ~SceneBase() override;

    bool OnCreate() override final;
    void OnEnter() override;
    void OnExit() override;
    void OnDestroy() override final;

    virtual void Update(float dt) override;
    virtual void SubmitObj() override;
    virtual void SubmitUI() override;

    const char* GetDebugName() const override { return "SceneBase"; }

protected:
    ObjectManager* GetObjectManager() const { return m_objectManager.get(); }

    // 파생 씬에서 구현 ㄱㄱ
    virtual bool OnCreateScene();
    virtual void OnDestroyScene();

protected:
    std::unique_ptr<ObjectManager> m_objectManager;
    float m_lastDt = 0.0f;
};
