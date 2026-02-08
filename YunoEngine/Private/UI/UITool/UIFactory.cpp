#include "pch.h"
#include "UIFactory.h"

#include "IWindow.h"
#include "UIConverter.h"
#include "YunoTransform.h"
//#include "YunoEngine.h"

UIFactory::~UIFactory() {}

Float2 UIFactory::GetCanvasSize() { return m_uiManager.GetCanvasSize(); }
UICanvasMapping UIFactory::GetCanvasMapping() { return m_uiManager.GetCanvasMapping(); }

float UIFactory::GetClientWidth() { return (float)YunoEngine::GetWindow()->GetClientWidth(); }
float UIFactory::GetClientHeight() { return (float)YunoEngine::GetWindow()->GetClientHeight(); }
Float2 UIFactory::GetClientSize() {
    return {
        (float)YunoEngine::GetWindow()->GetClientHeight(),
        (float)YunoEngine::GetWindow()->GetClientHeight()
    };
}
