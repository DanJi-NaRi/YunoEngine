#pragma once
#include "Widget.h"

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨

// 버튼 상태(Event Flag)
// Idle    : 기본값(상호작용 X)
// Hovered : 커서위치 버튼 위(Hoverd,Moved), 동시에 Up 역할도...
// Clicked : 이번 프레임에 버튼 누름
// Pressed : 버튼 누르고 있음
enum class ButtonState : int { Idle, Hovered, Clicked, Pressed, Count };

class Button : public Widget {
    public:
    explicit Button();
    virtual ~Button();

    //bool Create(XMFLOAT3 vPos) override;
    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    
    void ButtonUpdate(float dTime = 0);
    ButtonState GetButtonState() { return m_BtnState; }
private:
    //bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)
    //void SetMesh(std::unique_ptr<MeshNode>&& mesh);

    //추가 머테리얼 - 필요할까?
    //MaterialHandle m_addMaterial = 0;

private:

    // 버튼 상태 : 대기/커서입력/눌림 (standby/CursorOn/Push)
    ButtonState m_BtnState;							//UI버튼별 상태 저장.
    RECT m_rect;
};



/*
#define MS_STANDBY   0x0000		//대기중.
#define MS_LBTNDOWN  0x0100		//버튼 눌림.
#define MS_LBTNUP	 0x8100		//버튼 눌린후 떨어짐.
#define MS_RBTNDOWN  0x0001		//버튼 눌림.
#define MS_RBTNUP	 0x0081		//버튼 눌린후 떨어짐.
*/
