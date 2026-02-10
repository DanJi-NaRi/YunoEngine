#pragma once
#include "Button.h"

class IScene;

class PopButton : public Button
{
public:
    PopButton(UIFactory& uiFactory);
    virtual ~PopButton();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    // 버튼 이벤트 핸들
    virtual bool IdleEvent() override;                          // 아무것도 안할 때
    virtual bool HoveredEvent() override;                       // 커서가 위에 있을 때
    virtual bool LMBPressedEvent() override;                    // 왼클릭 눌렀을 때
    virtual bool RMBPressedEvent() override;                    // 우클릭 눌렀을 때
    virtual bool KeyPressedEvent(uint32_t key = 0) override;    // 바인딩한 키 눌렀을 때
    virtual bool LMBReleasedEvent() override;                   // 왼클릭 뗐을 때
    virtual bool RMBReleasedEvent() override;                   // 우클릭 뗐을 때
    virtual bool KeyReleasedEvent(uint32_t key = 0) override;   // 바인딩한 키 뗐을 때

    virtual void Exit(); // 나가기 버튼은 많으니 파생해서 쓸수도?

    void SetExitScene(IScene* scene) { m_pExitScene = scene; }
    IScene* GetExitScene() { return m_pExitScene; }

    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::PopButton; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/TITLE/exit_mouseout.png"); }    // 머테리얼 생성 (한 번만)
protected:
    IScene* m_pExitScene = nullptr;
    // SceneEnum m_ExitScene = SceneEnum::Default;
};
