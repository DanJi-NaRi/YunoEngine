#include "pch.h"
#include "PhaseWeaponSelectButton.h"

#include "YunoEngine.h"
#include "IInput.h"

#include "CardData.h"
#include "GameManager.h"


PhaseWeaponSelectButton::PhaseWeaponSelectButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

PhaseWeaponSelectButton::~PhaseWeaponSelectButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void PhaseWeaponSelectButton::Clear()
{
}

bool PhaseWeaponSelectButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    sizePx /= 2;
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    SetHoverTexture(L"../Assets/Test/PhaseStaminaBar_Noting_mouseout.png",
                     L"../Assets/Test/PhaseStaminaBar_Noting_mouseover.png");

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

bool PhaseWeaponSelectButton::Update(float dTime) {
    Button::Update(dTime);
    return true;
}

bool PhaseWeaponSelectButton::Submit(float dTime)
{
    Button::Submit(dTime);

    return true;
}

// 가만히 있을 때
bool PhaseWeaponSelectButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성

    return true;
}

// 커서가 위에 올라있을 때
bool PhaseWeaponSelectButton::HoveredEvent()
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
bool PhaseWeaponSelectButton::LMBPressedEvent()
{
    //std::cout << "(LMB)PressedEvent" << std::endl;
    // 게임 끄기
    return true;
}

// 우클릭 눌렀을 때
bool PhaseWeaponSelectButton::RMBPressedEvent()
{
    //std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool PhaseWeaponSelectButton::KeyPressedEvent(uint32_t key)
{
    /*if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;*/
    return true;
}

// 왼클릭 뗐을 때
bool PhaseWeaponSelectButton::LMBReleasedEvent()
{
    //std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool PhaseWeaponSelectButton::RMBReleasedEvent()
{
    //std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool PhaseWeaponSelectButton::KeyReleasedEvent(uint32_t key)
{
    /* if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
     else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;*/
    return true;
}

void PhaseWeaponSelectButton::ChangeWeaponImage(int id)
{
    switch (id) {
    case (int)PieceType::Blaster:   
        SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/blaster_mouseout.png",
                         L"../Assets/UI/PLAY/PhaseScene/blaster_mouseover.png");
        break;                       
    case (int)PieceType::Chakram:    
        SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/chakram_mouseout.png",
                         L"../Assets/UI/PLAY/PhaseScene/chakram_mouseover.png");
        break;                       
    case (int)PieceType::Breacher:   
        SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/breacher_mouseout.png",
                         L"../Assets/UI/PLAY/PhaseScene/breacher_mouseover.png");
        break;                       
    case (int)PieceType::Scythe:     
        SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/scythe_mouseout.png",
                         L"../Assets/UI/PLAY/PhaseScene/scythe_mouseover.png");
        break;                       
    case (int)PieceType::Impactor:   
        SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/impactor_mouseout.png",
                         L"../Assets/UI/PLAY/PhaseScene/impactor_mouseover.png");
        break;                       
    case (int)PieceType::Cleaver:    
        SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/cleaver_mouseout.png",
                         L"../Assets/UI/PLAY/PhaseScene/cleaver_mouseover.png");
        break;                       
    default:                         
        SetHoverTexture(L"../Assets/UI/PLAY/PhaseScene/test_non_mouseout.png",
                         L"../Assets/UI/PLAY/PhaseScene/test_non_mouseover.png");
        break;
    }
}
