#pragma once

#include "IScene.h"
#include "IAudioScene.h"
class ObjectManager;

class Unit;
class Widget;


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

    const char* GetDebugName() const override { return m_name.c_str(); }

#ifdef _DEBUG
    void DrawObjectListUI();
    void DrawInspector();
#endif

protected:
    ObjectManager* GetObjectManager() const { return m_objectManager.get(); }

#ifdef _DEBUG
    Unit* GetSelectedObject() const { return m_selectedObject; }
    void    SelectObject(Unit* obj) { m_selectedObject = obj; }
    Widget* GetSelectedWidget() const { return m_selectedWidget; }
    void    SelectWidget(Widget* obj) { m_selectedWidget = obj; }
#endif

    // 파생 씬에서 구현 ㄱㄱ
    virtual bool OnCreateScene();
    virtual void OnDestroyScene();

protected:
    std::unique_ptr<ObjectManager> m_objectManager;

#ifdef _DEBUG
    Unit* m_selectedObject;
    Widget* m_selectedWidget;
#endif
    float m_lastDt = 0.0f;
    std::string m_name;
    std::unique_ptr<IAudioScene> m_audioScene;

};
