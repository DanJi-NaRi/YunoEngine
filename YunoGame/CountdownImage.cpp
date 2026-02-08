#include "pch.h"
#include "CountdownImage.h"

#include "YunoEngine.h"
#include "IInput.h"

CountdownImage::CountdownImage(UIFactory& uiFactory) : Image(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

CountdownImage::~CountdownImage()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void CountdownImage::Clear()
{
}

bool CountdownImage::ChangeMaterial(int num)
{
    if (!m_MeshNode || m_MeshNode->m_Meshs.empty())
        return false;
    std::wstring texturePath = L"../Assets/Test/X.png";

    switch (num)
    {
    case 3:
        texturePath = L"../Assets/UI/WEAPON_SELECT/3.png";
        break;
    case 2:
        texturePath = L"../Assets/UI/WEAPON_SELECT/2.png";
        break;
    case 1:
        texturePath = L"../Assets/UI/WEAPON_SELECT/1.png";
        break;
    default:
        return false;
    }

    ChangeTexture(texturePath);
    return true;
}

bool CountdownImage::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);

    return true;
}

bool CountdownImage::Update(float dTime) {
    Image::Update(dTime);


    return true;
}

bool CountdownImage::Submit(float dTime)
{
    Image::Submit();
    return true;
}
