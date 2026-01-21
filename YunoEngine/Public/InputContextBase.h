#pragma once
#include "IInputContext.h"
#include "SceneBase.h"
#include "UIManager.h"
#include "ObjectManager.h"

class InputContextBase : public IInputContext
{
public:
    virtual int GetPriority() const override = 0;

    virtual bool OnInputEvent(InputEvent& evt) override = 0;

    virtual bool Event(InputEvent& evt) override;

    void SetScene(IScene* scene) {
        assert(scene);

        SceneBase* pScene = dynamic_cast<SceneBase*>(scene);
        if (!pScene) return;

        m_scene = pScene;
        m_objManager = m_scene->GetObjectManager();
        m_uiManager = m_scene->GetUIManager();
    }
    //void SetObjectManager(ObjectManager* objManger) { m_objManager = m_objManager; }
    //void SetUIManager(UIManager* uiManger) { m_uiManager = uiManger; }

    protected:
    SceneBase* m_scene = nullptr;
    ObjectManager* m_objManager = nullptr;
    UIManager* m_uiManager = nullptr;
};

