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

    void Update(float dt) override;
    void Submit() override;

    const char* GetDebugName() const override { return m_name.c_str(); }

    void DrawObjectListUI();

protected:
    ObjectManager* GetObjectManager() const { return m_objectManager.get(); }

    // 파생 씬에서 구현 ㄱㄱ
    virtual bool OnCreateScene();
    virtual void OnDestroyScene();

protected:
    std::unique_ptr<ObjectManager> m_objectManager;
    float m_lastDt = 0.0f;
    std::string m_name;
};
