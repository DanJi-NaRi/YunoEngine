#pragma once
#include "Widget.h"

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨

// 버튼 상태(Event Flag)
// Idle    : 기본값(상호작용 X)
// Hovered : 커서위치 버튼 위(Hovered,Moved), 동시에 Up 역할도...
// Clicked : 이번 프레임에 버튼 누름
// Pressed : 버튼 누르고 있음

// 카드도 Button 상속해서 쓰게 하면 될 듯??

enum class ButtonState : int { Idle, Hovered, Pressed, Down, Released, Count };
class Button : public Widget {
public:
    explicit Button();
    virtual ~Button();

    //bool Create(XMFLOAT3 vPos) override;
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    
    void ButtonUpdate(float dTime = 0);

    virtual bool IdleEvent();
    virtual bool HoveredEvent();
    virtual bool LMBPressedEvent();
    virtual bool RMBPressedEvent();
    virtual bool KeyPressedEvent(uint32_t key = 0);
    virtual bool DownEvent();
    virtual bool ReleasedEvent();


    ButtonState GetButtonState() { return m_BtnState; }
    void        SetButtonState(ButtonState state) { m_BtnState = state; }
    uint32_t    GetBindKey() { return m_Bindkey; }
    bool        IsBindkey() { return (m_Bindkey != 0); }
    void        SetBindKey(uint32_t bindkey) { m_Bindkey = bindkey; }
protected:
    // 버튼 상태 : 대기/커서입력/눌림 (standby/CursorOn/Push)
    ButtonState m_BtnState;							//UI버튼별 상태 저장.
    uint32_t m_Bindkey = 'A';
private:
    //bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)
    //void SetMesh(std::unique_ptr<MeshNode>&& mesh);

    //추가 머테리얼 - 필요할까?
    MaterialHandle m_addMaterial = 0;
};



/*
#define MS_STANDBY   0x0000		//대기중.
#define MS_LBTNDOWN  0x0100		//버튼 눌림.
#define MS_LBTNUP	 0x8100		//버튼 눌린후 떨어짐.
#define MS_RBTNDOWN  0x0001		//버튼 눌림.
#define MS_RBTNUP	 0x0081		//버튼 눌린후 떨어짐.
*/
