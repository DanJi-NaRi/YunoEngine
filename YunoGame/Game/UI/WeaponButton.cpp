#include "pch.h"

#include "WeaponButton.h"

#include "YunoEngine.h"
#include "IInput.h"
#include "GameManager.h"
#include "TextureImage.h"

namespace
{
    std::wstring GetPieceNameLower(PieceType pieceType)
    {
        switch (pieceType)
        {
        case PieceType::Blaster:
            return L"blaster";
        case PieceType::Breacher:
            return L"breacher";
        case PieceType::Impactor:
            return L"impactor";
        case PieceType::Chakram:
            return L"chakram";
        case PieceType::Scythe:
            return L"scythe";
        case PieceType::Cleaver:
            return L"cleaver";
        case PieceType::None:
        default:
            return L"";
        }
    }

    std::wstring GetSelectedSlotTexturePath(PieceType pieceType, int slotTextureIndex)
    {
        const std::wstring pieceName = GetPieceNameLower(pieceType);
        if (pieceName.empty())
            return L"";

        if (slotTextureIndex < 1 || slotTextureIndex > 4)
            return L"";

        return L"../Assets/UI/WEAPON_SELECT/selected_" + pieceName + L"_" + std::to_wstring(slotTextureIndex) + L".png";
    }

    std::wstring GetWeaponSlotTexturePath(PieceType pieceType, int slotTextureIndex)
    {
        const std::wstring pieceName = GetPieceNameLower(pieceType);
        if (pieceName.empty())
            return L"";

        if (slotTextureIndex < 1 || slotTextureIndex > 4)
            return L"";

        return L"../Assets/UI/WEAPON_SELECT/weapon_" + std::to_wstring(slotTextureIndex) + L"_" + pieceName + L".png";
    }

    std::wstring GetWeaponNameTexturePath(PieceType pieceType)
    {
        const std::wstring pieceName = GetPieceNameLower(pieceType);
        if (pieceName.empty())
            return L"";

        return L"../Assets/UI/WEAPON_SELECT/Weapon_name_" + pieceName + L".png";
    }

    std::wstring GetWeaponCardTexturePath(PieceType pieceType)
    {
        const std::wstring pieceName = GetPieceNameLower(pieceType);
        if (pieceName.empty())
            return L"";
    
        return L"../Assets/UI/CARD/card_" + pieceName + L"_3.png";
    }
}

WeaponButton::WeaponButton(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
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

bool WeaponButton::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    sizePx.x /= 2;
    sizePx.y /= 2;
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_bindkey = 0; // 0인 경우, 안쓴다는 뜻

    return true;
}

bool WeaponButton::Update(float dTime) {
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
    std::cout << "OnCLick" << std::endl;
    const int myIdx = GameManager::Get().GetSlotiIdx();
    if (myIdx != 1 && myIdx != 2)
        return true;


    TextureImage* slotImage0 = nullptr;
    TextureImage* slotImage1 = nullptr;

    if (myIdx == 1)
    {
        slotImage0 = dynamic_cast<TextureImage*>(m_pUserImage0);
        slotImage1 = dynamic_cast<TextureImage*>(m_pUserImage1);
    }
    else
    {
        slotImage0 = dynamic_cast<TextureImage*>(m_pUserImage2);
        slotImage1 = dynamic_cast<TextureImage*>(m_pUserImage3);
    }

    if (!slotImage0 || !slotImage1)
        return true;



    TextureImage* targetImage = nullptr;
    TextureImage* targetWeaponImage = nullptr;
    int pickIndex = 0;

    if (GameManager::Get().GetMyPiece(0) == PieceType::None)
    {
        targetImage = slotImage0;
        targetWeaponImage = (myIdx == 1)
            ? dynamic_cast<TextureImage*>(m_pWeaponImage0)
            : dynamic_cast<TextureImage*>(m_pWeaponImage2);
        pickIndex = 0;
    }
    else if (GameManager::Get().GetMyPiece(1) == PieceType::None)
    {
        targetImage = slotImage1;
        targetWeaponImage = (myIdx == 1)
            ? dynamic_cast<TextureImage*>(m_pWeaponImage1)
            : dynamic_cast<TextureImage*>(m_pWeaponImage3);
        pickIndex = 1;
    }
    else
    {
        targetImage = slotImage0;
        targetWeaponImage = (myIdx == 1)
            ? dynamic_cast<TextureImage*>(m_pWeaponImage0)
            : dynamic_cast<TextureImage*>(m_pWeaponImage2);
        pickIndex = 0;
    }

    const int slotTextureIndex = (myIdx == 1 ? 1 : 3) + pickIndex;
    const std::wstring slotTexturePath = GetSelectedSlotTexturePath(m_pieceType, slotTextureIndex);
    if (!slotTexturePath.empty())
    {
        targetImage->ChangeTexture(slotTexturePath);
    }

    const std::wstring weaponTexturePath = GetWeaponSlotTexturePath(m_pieceType, slotTextureIndex);
    if (targetWeaponImage && !weaponTexturePath.empty())
    {
        targetWeaponImage->ChangeTexture(weaponTexturePath);
        targetWeaponImage->SetScale(XMFLOAT3(1.f, 1.f, 1.f));
    }

    TextureImage* weaponNameImage = dynamic_cast<TextureImage*>(m_pWeaponNameImage);
    if (weaponNameImage)
    {
        const std::wstring weaponNameTexturePath = GetWeaponNameTexturePath(m_pieceType);
        if (!weaponNameTexturePath.empty())
        {
            weaponNameImage->ChangeTexture(weaponNameTexturePath);
        }
    }

    TextureImage* weaponCardImage = dynamic_cast<TextureImage*>(m_pWeaponCardImage);
    if (weaponCardImage)
    {
        const std::wstring weaponCardTexturePath = GetWeaponCardTexturePath(m_pieceType);
        if (!weaponCardTexturePath.empty())
        {
            weaponCardImage->ChangeTexture(weaponCardTexturePath);
        }
    }

    GameManager::Get().SetMyPick(pickIndex, m_pieceType);

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

bool WeaponButton::SetPieceType(PieceType pieceType)
{
    m_pieceType = pieceType; 
    std::wstring texturePath = L"../Assets/Test/X.png";
    switch (m_pieceType)
    {
    case PieceType::Blaster:
        texturePath = L"../Assets/Test/unit1.png";
        break;
    case PieceType::Breacher:
        texturePath = L"../Assets/Test/unit2.png";
        break;
    case PieceType::Impactor:
        texturePath = L"../Assets/Test/unit3.png";
        break;
    case PieceType::Chakram:
        texturePath = L"../Assets/Test/unit4.png";
        break;
    case PieceType::Scythe:
        texturePath = L"../Assets/Test/unit5.png";
        break;
    case PieceType::Cleaver:
        texturePath = L"../Assets/Test/unit6.png";
        break;
    case PieceType::None:
    default:
        break;
    }

    ChangeTexture(texturePath);
    //m_MeshNode->m_Meshs[0]->SetTexture(TextureUse::Albedo, texturePath);


    return true;
}

void WeaponButton::SetUserImages(Widget* U1I1, Widget* U1I2, Widget* U2I1, Widget* U2I2)
{
    m_pUserImage0 = U1I1;
    m_pUserImage1 = U1I2;
    m_pUserImage2 = U2I1;
    m_pUserImage3 = U2I2;
}

void WeaponButton::SetWeaponImages(Widget* U1W1, Widget* U1W2, Widget* U2W1, Widget* U2W2)
{
    m_pWeaponImage0 = U1W1;
    m_pWeaponImage1 = U1W2;
    m_pWeaponImage2 = U2W1;
    m_pWeaponImage3 = U2W2;
}

void WeaponButton::SetWeaponPreviewImages(Widget* weaponNameImage, Widget* weaponCardImage)
{
    m_pWeaponNameImage = weaponNameImage;
    m_pWeaponCardImage = weaponCardImage;
}
