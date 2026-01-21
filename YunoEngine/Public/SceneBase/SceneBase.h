#pragma once

#include "IScene.h"
#include "IAudioScene.h"
class ObjectManager;
class UIManager;
class IInput;
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
    void DrawObjectList();
    void DrawUIList();
    void DrawInspector();
#endif

    ObjectManager* GetObjectManager() const { return m_objectManager.get(); }
    UIManager* GetUIManager() const { return m_uiManager.get(); }

protected:
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
    std::unique_ptr<UIManager> m_uiManager;
    IInput* m_input = nullptr;

#ifdef _DEBUG
    Unit* m_selectedObject;
    Widget* m_selectedWidget;
#endif
    float m_lastDt = 0.0f;
    std::string m_name;
    std::unique_ptr<IAudioScene> m_audioScene;

};
