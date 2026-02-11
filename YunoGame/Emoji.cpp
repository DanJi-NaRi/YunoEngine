#include "pch.h"
#include "Emoji.h"

Emoji::Emoji(UIFactory& uiFactory)
    : Image(uiFactory)
{
    Clear(); // Clear 추가는 기본적으로!!
}

Emoji::~Emoji()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void Emoji::Clear()
{
}

bool Emoji::ChangeMaterial(int num)
{
    if (!m_MeshNode || m_MeshNode->m_Meshs.empty())
    {
        return false;
    }

    std::wstring texturePath = L"../Assets/Test/X.png";

    std::cout << "sird\n";

    switch (num)
    {
    case 1:
        texturePath = L"../Assets/UI/PLAY/Imoji_zZZ_mouseover.png";
        break;
    case 2:
        texturePath = L"../Assets/UI/PLAY/Imoji_JiantAngry_mouseover.png";
        break;
    case 3:
        texturePath = L"../Assets/UI/PLAY/Imoji_EZ_mouseover.png";
        break;
    default:
        break;
    }

    ChangeTexture(texturePath);
    //m_MeshNode->m_Meshs[0]->SetTexture(TextureUse::Albedo, texturePath);
    return true;
}

bool Emoji::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);
    return true;
}

bool Emoji::Update(float dTime) 
{
    Image::Update(dTime);
    return true;
}


bool Emoji::Submit(float dTime)
{
    Image::Submit();
    return true;
}
