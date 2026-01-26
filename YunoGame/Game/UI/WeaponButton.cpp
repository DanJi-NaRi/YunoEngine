#include "pch.h"

#include "WeaponButton.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "GameManager.h"
#include "UserImage.h"

WeaponButton::WeaponButton(UIManager* uiManager) : Button(uiManager) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

WeaponButton::~WeaponButton()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void WeaponButton::Clear()
{
}

bool WeaponButton::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Button::Create(name, id, vPos);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

bool WeaponButton::Update(float dTime)
{
    Button::Update(dTime);
    return true;
}

bool WeaponButton::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool WeaponButton::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool WeaponButton::HoveredEvent()
{
    //std::cout << "HoveredEvent" << std::endl;
    return true;
}

// Down 기능은 현재 미지원
//bool WeaponButton::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool WeaponButton::LMBPressedEvent()
{
    const int myIdx = GameManager::Get().GetSlotiIdx();
    if (myIdx != 0 && myIdx != 1)
        return true;

    UserImage* slotImage0 = nullptr;
    UserImage* slotImage1 = nullptr;

    if (myIdx == 0)
    {
        slotImage0 = dynamic_cast<UserImage*>(m_pUserImage0);
        slotImage1 = dynamic_cast<UserImage*>(m_pUserImage1);
    }
    else
    {
        slotImage0 = dynamic_cast<UserImage*>(m_pUserImage2);
        slotImage1 = dynamic_cast<UserImage*>(m_pUserImage3);
    }

    if (!slotImage0 || !slotImage1)
        return true;

    UserImage* targetImage = nullptr;
    if (slotImage0->GetPieceType() == PieceType::None)
        targetImage = slotImage0;
    else if (slotImage1->GetPieceType() == PieceType::None)
        targetImage = slotImage1;
    else
        targetImage = slotImage0;

    targetImage->SetPieceType(m_pieceType);
    targetImage->ChangeMaterial(static_cast<int>(m_pieceType));

    return true;
}

// 우클릭 눌렀을 때
bool WeaponButton::RMBPressedEvent()
{
    //std::cout << "(RMB)PressedEvent" << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool WeaponButton::KeyPressedEvent(uint32_t key)
{
    //if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    //else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool WeaponButton::LMBReleasedEvent()
{
    //std::cout << "(LMB)ReleasedEvent" << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool WeaponButton::RMBReleasedEvent()
{
    //std::cout << "(RMB)ReleasedEvent" << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool WeaponButton::KeyReleasedEvent(uint32_t key)
{
    //if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    //else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}

void WeaponButton::SetUserImages(Widget* U1I1, Widget* U1I2, Widget* U2I1, Widget* U2I2)
{
    m_pUserImage0 = U1I1;
    m_pUserImage1 = U1I2;
    m_pUserImage2 = U2I1;
    m_pUserImage3 = U2I2;
}


