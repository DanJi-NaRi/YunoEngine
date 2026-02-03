#include "pch.h"
#include "CardConfirmButton.h"

#include "YunoEngine.h"
#include "IInput.h"

CardConfirmButton::CardConfirmButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

CardConfirmButton::~CardConfirmButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void CardConfirmButton::Clear()
{
}

bool CardConfirmButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

void CardConfirmButton::CreateChild()
{
}

bool CardConfirmButton::Update(float dTime)
{
    Button::Update(dTime);
    return true;
}

bool CardConfirmButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool CardConfirmButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool CardConfirmButton::HoveredEvent()
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
bool CardConfirmButton::LMBPressedEvent()
{
    std::cout << "(LMB)CardConfirmButton Pressed" << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool CardConfirmButton::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool CardConfirmButton::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool CardConfirmButton::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool CardConfirmButton::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool CardConfirmButton::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}
