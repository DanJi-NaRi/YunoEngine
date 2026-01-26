#include "pch.h"
#include "UserImage.h"

#include "YunoEngine.h"
#include "IInput.h"

UserImage::UserImage(UIManager* uiManager) : Image(uiManager) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
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
    return true;
}

bool UserImage::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Image::Create(name, id, vPos);
    m_userJoined = false;
    m_selectPiece = PieceType::None;
    return true;
}

bool UserImage::Update(float dTime)
{
    Image::Update(dTime);
    return true;
}

bool UserImage::Submit(float dTime)
{
    Image::Submit();
    return true;
}
