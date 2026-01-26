#include "pch.h"
#include "ReadyButton.h"

#include "YunoEngine.h"
#include "IInput.h"

ReadyButton::ReadyButton(UIManager* uiManager) : Button(uiManager) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

ReadyButton::~ReadyButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void ReadyButton::Clear()
{
    m_isReady = false;
}

bool ReadyButton::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

bool ReadyButton::Update(float dTime)
{
    Button::Update(dTime);
    return true;
}

bool ReadyButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool ReadyButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool ReadyButton::HoveredEvent()
{
    std::cout << "HoveredEvent" << std::endl;
    return true;
}

// Down 기능은 현재 미지원
//bool ButtonTemplate::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool ReadyButton::LMBPressedEvent()
{
    std::cout << "(LMB)PressedEvent" << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool ReadyButton::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool ReadyButton::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool ReadyButton::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool ReadyButton::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool ReadyButton::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

