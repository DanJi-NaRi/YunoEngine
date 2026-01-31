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
        return m_uiManager.CreateWidget<T>(name, pos); // UIManager 멤버 접근은 내부에서
    }
     

    // 자식 생성
    template<typename T>
    T* CreateChild(const std::wstring& name, DirectX::XMFLOAT3 pos, Widget* pParent) // 위젯 생성 // 템플릿 함수
    {
        assert(pParent);
        if (!pParent) return nullptr;

         T* pChild = m_uiManager.CreateWidget<T>(name, pos); // UIManager 멤버 접근은 내부에서
         pParent->Attach(pChild);
         pChild->SetLayer(pParent->GetLayer());

         if (!pChild) return nullptr;

         return pChild;
    }
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

