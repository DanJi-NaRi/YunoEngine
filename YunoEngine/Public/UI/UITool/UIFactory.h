#pragma once
#include <string>
#include <DirectXMath.h>
#include "UIManager.h"
class UIManager;
class Float2;

//Widget한테 외부 매니저 클래스의 일부 기능을 공급하는 팩토리
class UIFactory
{
public:
    UIFactory();
    explicit UIFactory(UIManager& uiManager) : m_uiManager(uiManager) {}
    ~UIFactory();

    //===================================//
    // UIManager.h
    Float2 GetCanvasSize(); // 캔버스 사이즈 or 클라이언트 사이즈 반환

    template<typename T>
    T* CreateWidget(const std::wstring& name, DirectX::XMFLOAT3 pos) // 위젯 생성 // 템플릿 함수
    {
        static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
        assert(m_uiManager);
        Widget* pWidget = m_uiManager.CreateWidget<T>(name, pos); // UIManager 멤버 접근은 내부에서
        pWidget->CreateChild();
        return pWidget;
    }
    template<typename T>
    T* CreateWidget(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, UIDirection dir, Widget* pParent) {
        static_assert(std::is_base_of_v<Widget, T>, "T must derive from Widget");
        assert(m_uiManager);
        Widget* pWidget = m_uiManager.CreateWidget<T>(name, pos); // UIManager 멤버 접근은 내부에서
        pWidget->CreateChild();
        return pWidget;
    }

     
    // 자식 생성 (부모 레이어 초회 상속)
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, Widget* pParent);
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, Float2 pivot, Widget* pParent);
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, UIDirection dir, Widget* pParent);
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, Float2 pivot, Widget* pParent);
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, UIDirection dir, Widget* pParent);
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, float rot, DirectX::XMFLOAT3 scale, Float2 pivot, Widget* pParent);
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, float rot, DirectX::XMFLOAT3 scale, UIDirection dir, Widget* pParent);
    
    // 유일하게 레이어 따로 설정
    template<typename T>
    T* CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, UIDirection pivot, WidgetLayer layer, Widget* pParent) ;
    //===================================//


    //===================================//
    // IWindow.h
    float GetClientWidth();
    float GetClientHeight();
    Float2 GetClientSize();
    //===================================//

private:
    UIManager& m_uiManager;
};

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos); // UIManager 멤버 접근은 내부에서
    //pChild->SetLayer(pParent->GetLayer());
   
    pParent->Attach(pChild);
    pChild->CreateChild(); // 자식 생성
    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, Float2 pivot, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos, pivot); // UIManager 멤버 접근은 내부에서
    //pChild->SetLayer(pParent->GetLayer());
    pChild->CreateChild(); // 자식 생성
    pParent->Attach(pChild);
    
    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, UIDirection pivot, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos, pivot); // UIManager 멤버 접근은 내부에서
    pParent->Attach(pChild);
    //pChild->SetLayer(pParent->GetLayer());
    pChild->CreateChild(); // 자식 생성
    
    
    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, Float2 pivot, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos, pivot); // UIManager 멤버 접근은 내부에서
    pParent->Attach(pChild);
    //pChild->SetLayer(pParent->GetLayer());
    pChild->CreateChild(); // 자식 생성


    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, UIDirection pivot, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos, pivot); // UIManager 멤버 접근은 내부에서
    pParent->Attach(pChild);
    //pChild->SetLayer(pParent->GetLayer());
    pChild->CreateChild(); // 자식 생성


    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, float rot, DirectX::XMFLOAT3 scale, Float2 pivot, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos, pivot); // UIManager 멤버 접근은 내부에서
    pParent->Attach(pChild);
    //pChild->SetLayer(pParent->GetLayer());
    pChild->CreateChild(); // 자식 생성


    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, float rot, DirectX::XMFLOAT3 scale, UIDirection pivot, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos, pivot); // UIManager 멤버 접근은 내부에서
    pParent->Attach(pChild);
    //pChild->SetLayer(pParent->GetLayer());
    pChild->CreateChild(); // 자식 생성


    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

template<typename T>
T* UIFactory::CreateChild(const std::wstring& name, Float2 sizePx, DirectX::XMFLOAT3 pos, UIDirection pivot, WidgetLayer layer, Widget* pParent) {
    assert(pParent);
    if (!pParent) return nullptr;

    T* pChild = m_uiManager.CreateWidget<T>(name, sizePx, pos, pivot); // UIManager 멤버 접근은 내부에서
    pParent->Attach(pChild);
    pChild->SetLayer(layer);
    pChild->CreateChild(); // 자식 생성


    //pChild->SetPivot(pParent->GetPivot());

    assert(pChild);
    if (!pChild) return nullptr;

    return pChild;
}

