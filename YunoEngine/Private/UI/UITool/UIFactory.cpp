#include "pch.h"
#include "UIFactory.h"

#include "UIManager.h"
#include "YunoTransform.h"


UIFactory::UIFactory(UIManager& uiManager) : m_uiManager(uiManager) {}

template<typename T>
T* UIFactory::CreateWidget(const std::wstring& name, XMFLOAT3 pos)
{
    return m_uiManager.CreateWidget<T>(name, pos); // UIManager 멤버 접근은 내부에서
}

Float2 UIFactory::GetCanvasSize() { return m_uiManager.GetCanvasSize(); }
