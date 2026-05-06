#include "pch.h"
#include "DirSelectButton.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "UIFactory.h"

//#include "DragProvider.h" // 드래그가 필요하면

DirSelectButton::DirSelectButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear();
}

DirSelectButton::~DirSelectButton()
{
    Clear();
}

void DirSelectButton::Clear()
{
}

bool DirSelectButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);


    SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/direction_mouseout.png", L"../Assets/UI/PLAY/PhaseScene/direction_mouseover.png");

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

void DirSelectButton::CreateChild()
{
    // 자식 생성 공간, 고정 하위 위젯 생성

}

bool DirSelectButton::Start()
{
    Button::Start();
    return true;
}

bool DirSelectButton::Update(float dTime) {
    Button::Update(dTime);
    return true;
}

bool DirSelectButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool DirSelectButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool DirSelectButton::HoveredEvent()
{
    //std::cout << "HoveredEvent" << std::endl;
    return true;
}

// Down 기능은 현재 미지원
//bool ButtonTemplate::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool DirSelectButton::LMBPressedEvent()
{
    std::cout << "(LMB)PressedEvent" << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool DirSelectButton::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool DirSelectButton::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool DirSelectButton::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool DirSelectButton::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool DirSelectButton::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}
