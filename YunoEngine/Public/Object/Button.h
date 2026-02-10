#pragma once
#include "Widget.h"

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨

// 버튼 상태(Event Flag)
// Idle    : 기본값(상호작용 X)
// Hovered : 커서위치 버튼 위(Hovered,Moved), 동시에 Up 역할도...
// Pressed : 이번 프레임에 버튼 누름
// Down : 버튼 누르고 있음

// 카드도 Button 상속해서 쓰게 하면 될 듯??

class DragProvider;

enum class ButtonState : int { Idle, Hovered, Pressed, Down, Released, Count };

class Button : public Widget {
public:
    explicit Button(UIFactory& uiFactory);
    virtual ~Button();

    //bool Create(XMFLOAT3 vPos) override;
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    
    void ButtonUpdate(float dTime = 0);

    const std::wstring& GetHoveredTexturePath() { return m_hoverdTexturePath; }

    void SetHoverTexture(std::wstring idlePath, std::wstring hoverdPath);

    void SetIdleTexture(std::wstring path);
    void SetHoverdTexture(std::wstring path);
    void SetClickedTexture(std::wstring path);

    virtual bool IdleEvent();
    virtual bool HoveredEvent();
    virtual bool LMBPressedEvent();
    virtual bool RMBPressedEvent();
    virtual bool KeyPressedEvent(uint32_t key = 0);
    virtual bool LMBReleasedEvent();
    virtual bool RMBReleasedEvent();
    virtual bool KeyReleasedEvent(uint32_t key = 0);
    virtual bool DownEvent();
    
    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Button; }
    std::function<void()>& GetEventLMB() { return m_eventLMB; } // LeftPress기준
    std::function<void()>& GetEventRMB() { return m_eventRMB; } // RightPress기준

    DragProvider* GetDragProvider() { return m_pDrag.get(); } // 없으면 nullptr 반환

    void        SetButtonState(ButtonState state) { m_btnState = state; }
    void        SetBindKey(uint32_t bindkey) { m_bindkey = bindkey; }
    void        SetEventLMB(std::function<void()> event) { m_eventLMB = std::move(event); }
    void        SetEventRMB(std::function<void()> event) { m_eventRMB = std::move(event); }

    ButtonState GetButtonState() const { return m_btnState; }
    uint32_t    GetBindKey() const { return m_bindkey; }
    bool        IsBindkey() const { return (m_bindkey != 0); }
    
    void        SetUseHoverPath(bool use) { m_useHoverPath = use; }
    bool        IsUseHoverPath() { return m_useHoverPath; }


    void        Clear();
protected:
    // 버튼 상태 : 대기/커서입력/눌림 (standby/CursorOn/Push)
    ButtonState m_btnState;							//UI버튼별 상태 저장.
    uint32_t m_bindkey = 0;
    std::unique_ptr<DragProvider> m_pDrag = nullptr; // 드래그 기능 공급자. 기본은 null이니 파생해서 초기화 때 채워줄 것.

    std::wstring m_hoverdTexturePath = L"000000"; // 커서를 올렸을 때, 바뀌는 텍스처
    std::wstring m_PressedTexturePath = L"000000"; // 클릭했을 때, 바뀌는 텍스처 // 사용 안 함
    bool m_useHoverPath = false;

    std::function<void()> m_eventLMB = nullptr; // 펑션 // 타 클래스의 함수를 등록하고 싶으면 사용
    std::function<void()> m_eventRMB = nullptr; // 펑션 // 타 클래스의 함수를 등록하고 싶으면 사용
private:
    //bool CreateMesh() override;      // 메시 생성 (한 번만)
    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/Textures/woodbox.bmp"); };    // 머테리얼 생성 (한 번만)
    //void SetMesh(std::unique_ptr<MeshNode>&& mesh);

};



/*
#define MS_STANDBY   0x0000		//대기중.
#define MS_LBTNDOWN  0x0100		//버튼 눌림.
#define MS_LBTNUP	 0x8100		//버튼 눌린후 떨어짐.
#define MS_RBTNDOWN  0x0001		//버튼 눌림.
#define MS_RBTNUP	 0x0081		//버튼 눌린후 떨어짐.
*/
