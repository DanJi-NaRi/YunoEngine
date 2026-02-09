#pragma once

#include "IScene.h"

#include <type_traits>
#include "Widget.h"
#include "UIManager.h"

class ObjectManager;
//class UIManager;
class EffectManager;
class IInput;
class Unit;
//class Widget;
class Light;


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

    template<typename T> // 모든 씬 공용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos);
    template<typename T> // 모든 씬 공용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, Float2 pivot);
    template<typename T> // 모든 씬 공용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, UIDirection pivot);
    template<typename T> // 모든 씬 공용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, XMFLOAT3 scale, Float2 pivot);
    template<typename T> // 모든 씬 공용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, XMFLOAT3 scale, UIDirection pivot);
    template<typename T> // 모든 씬 공용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, float rot, XMFLOAT3 scale, Float2 pivot);
    template<typename T> // 모든 씬 공용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, float rot, XMFLOAT3 scale, UIDirection pivot);

    template<typename T> // 텍스처 경로 지정 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, const std::wstring& texturePath, Float2 sizePx, XMFLOAT3 pos);
    template<typename T> // 텍스처 경로 + 피벗 지정 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, const std::wstring& texturePath, Float2 sizePx, XMFLOAT3 pos, UIDirection pivot);
    template<typename T> // 텍스처 원본 크기 자동 적용 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, const std::wstring& texturePath, XMFLOAT3 pos);
    template<typename T> // 텍스처 원본 크기 자동 적용 + 피벗 지정 Widget 생성 함수
    T* CreateWidget(const std::wstring& name, const std::wstring& texturePath, XMFLOAT3 pos, UIDirection pivot);

    UIManager* GetUIManager() { return m_uiManager.get(); }

#ifdef _DEBUG
    void DrawObjectList();
    void DrawObjectNode(Unit* obj);
    void DrawWidgetNode(Widget* obj);
    void DrawInspector();
#endif

    ObjectManager* GetObjectManager() const { return m_objectManager.get(); }
    UIManager* GetUIManager() const { return m_uiManager.get(); }

protected:
#ifdef _DEBUG
    Unit*   GetSelectedObject() const { return m_selectedObject; }
    void    SelectObject(Unit* obj) { m_selectedLight = nullptr;  m_selectedWidget = nullptr;  m_selectedObject = obj; }
    Widget* GetSelectedWidget() const { return m_selectedWidget; }
    void    SelectWidget(Widget* obj) { m_selectedLight = nullptr; m_selectedObject = nullptr;  m_selectedWidget = obj; }
    void    SelectLight(Light* obj) { m_selectedObject = nullptr; m_selectedWidget = nullptr; m_selectedLight = obj; }
#endif

    // 파생 씬에서 구현 ㄱㄱ
    bool LoadScene(const std::wstring& filepath, SceneDesc& out);
    virtual bool OnCreateScene();
    virtual void OnDestroyScene();

    virtual SceneDesc BuildSceneDesc() override;

protected:
    std::unique_ptr<ObjectManager> m_objectManager;
    std::unique_ptr<UIManager> m_uiManager;
    std::unique_ptr<EffectManager> m_effectManager;
    IInput* m_input = nullptr;

#ifdef _DEBUG
    Unit* m_selectedObject;
    Widget* m_selectedWidget;
    Light* m_selectedLight;
#endif
    float m_lastDt = 0.0f;
    std::string m_name;
};





/////
template<typename T> // 모든 씬 공용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, sizePx, pos);
    pWidget->CreateChild(); // 자식 생성
    return pWidget;
}
template<typename T> // 모든 씬 공용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, Float2 pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, sizePx, pos, pivot);
    pWidget->CreateChild(); // 자식 생성
    return pWidget;
}
template<typename T> // 모든 씬 공용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos,  UIDirection pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, sizePx, pos, pivot);
    pWidget->CreateChild(); // 자식 생성
    return pWidget;
}
template<typename T> // 모든 씬 공용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, XMFLOAT3 scale, Float2 pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, sizePx, pos, scale, pivot);
    pWidget->CreateChild(); // 자식 생성
    return pWidget;
}
template<typename T> // 모든 씬 공용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, XMFLOAT3 scale, UIDirection pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, sizePx, pos, scale, pivot);
    pWidget->CreateChild(); // 자식 생성
    return pWidget;
}
template<typename T> // 모든 씬 공용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, float rot, XMFLOAT3 scale, Float2 pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, sizePx, pos, rot, scale, pivot);
    pWidget->CreateChild(); // 자식 생성
    return pWidget;
}
template<typename T> // 모든 씬 공용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, Float2 sizePx, XMFLOAT3 pos, float rot, XMFLOAT3 scale, UIDirection pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, sizePx, pos, rot, scale, pivot);
    pWidget->CreateChild(); // 자식 생성
    return pWidget;
}

template<typename T> // 텍스처 경로 지정 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, const std::wstring& texturePath, Float2 sizePx, XMFLOAT3 pos)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, texturePath, sizePx, pos);
    pWidget->CreateChild();
    return pWidget;
}

template<typename T> // 텍스처 경로 + 피벗 지정 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, const std::wstring& texturePath, Float2 sizePx, XMFLOAT3 pos, UIDirection pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, texturePath, sizePx, pos, pivot);
    pWidget->CreateChild();
    return pWidget;
}

template<typename T> // 텍스처 원본 크기 자동 적용 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, const std::wstring& texturePath, XMFLOAT3 pos)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, texturePath, pos);
    pWidget->CreateChild();
    return pWidget;
}

template<typename T> // 텍스처 원본 크기 자동 적용 + 피벗 지정 Widget 생성 함수
T* SceneBase::CreateWidget(const std::wstring& name, const std::wstring& texturePath, XMFLOAT3 pos, UIDirection pivot)
{
    static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
    assert(m_uiManager);

    T* pWidget = m_uiManager->CreateWidget<T>(name, texturePath, pos, pivot);
    pWidget->CreateChild();
    return pWidget;
}
