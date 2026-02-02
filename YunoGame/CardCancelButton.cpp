#include "pch.h"
#include "CardCancelButton.h"

#include "YunoEngine.h"
#include "IInput.h"

CardCancelButton::CardCancelButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

CardCancelButton::~CardCancelButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void CardCancelButton::Clear()
{
}

bool CardCancelButton::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

void CardCancelButton::CreateChild()
{
}

bool CardCancelButton::Update(float dTime)
{
    Button::Update(dTime);
    return true;
}

bool CardCancelButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool CardCancelButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool CardCancelButton::HoveredEvent()
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
bool CardCancelButton::LMBPressedEvent()
{
    std::cout << "(LMB)CardCancelButton Pressed" << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool CardCancelButton::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool CardCancelButton::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool CardCancelButton::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool CardCancelButton::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool CardCancelButton::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}
