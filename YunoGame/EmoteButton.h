#pragma once
#include "Button.h"

class EmoteButton : public Button
{
public:
    // 생성자는 (UIFactory& uiFactory) 인자를 기본으로
    EmoteButton(UIFactory& uiFactory);
    virtual ~EmoteButton();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;                // 위젯 생성 동시에 바로 다음 차례에 자식 위젯을 생성해야 한다면 (자식 생성 공간)
    bool Start() override;                      // 생성 이후, 시작 지점 (자식 객체를 생성한다면 여기에)
    bool Update(float dTime = 0) override;      // UIManager가 전체 순회 업데이트 (Logic)
    bool Submit(float dTime = 0) override;
    void Clear();

    void Lock() { isLock = true; }
    void UnLock() { isLock = false; }

    void SetIdleHoverTextuer(const std::wstring& idlepath, const std::wstring& hoverpath);

    void SetEmoteNum(int num) { if (num < 0 || num > 3) return;  emoteNum = num; }

    // 버튼 이벤트 핸들
    virtual bool IdleEvent() override;                          // 아무것도 안할 때
    virtual bool HoveredEvent() override;                       // 커서가 위에 있을 때
    virtual bool LMBPressedEvent() override;                    // 왼클릭 눌렀을 때
    virtual bool RMBPressedEvent() override;                    // 우클릭 눌렀을 때

    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Card; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/woodbox.bmp"); };    // 머테리얼 생성 (한 번만)
protected:
    int emoteNum = -1;
    bool isLock = false;
};

