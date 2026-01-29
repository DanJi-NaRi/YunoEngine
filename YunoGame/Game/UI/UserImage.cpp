#include "pch.h"
#include "UserImage.h"

#include "YunoEngine.h"
#include "IInput.h"

UserImage::UserImage(UIFactory& uiFactory) : Image(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

UserImage::~UserImage()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void UserImage::Clear()
{
}

bool UserImage::ChangeMaterial(int num)
{
    if (!m_MeshNode || m_MeshNode->m_Meshs.empty())
        return false;

    std::wstring texturePath = L"../Assets/Test/X.png";
    switch (static_cast<PieceType>(num))
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

    m_MeshNode->m_Meshs[0]->SetTexture(TextureUse::Albedo, texturePath);
    return true;
}

bool UserImage::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Image::Create(name, id, vPos);
    //m_vScale = XMFLOAT3(0.01f, 0.01f, 0.01f);
    m_userJoined = false;
    m_selectPiece = PieceType::None;
    return true;
}

bool UserImage::UpdateTransform(float dTime = 0) {
    Image::UpdateTransform(dTime);

    return true;
}
bool UserImage::UpdateLogic(float dTime = 0) {

    return true;
}

bool UserImage::Submit(float dTime)
{
    Image::Submit();
    return true;
}
