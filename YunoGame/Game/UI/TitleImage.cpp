#include "pch.h"
#include "TitleImage.h"

#include "YunoEngine.h"
#include "IInput.h"

TitleImage::TitleImage(UIFactory& uiFactory) : Image(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

TitleImage::~TitleImage()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void TitleImage::Clear()
{
}

bool TitleImage::ChangeMaterial(int num)
{
    if (!m_MeshNode || m_MeshNode->m_Meshs.empty())
        return false;

    //std::wstring texturePath = L"../Assets/Test/X.png";

    //m_MeshNode->m_Meshs[0]->SetTexture(TextureUse::Albedo, texturePath);
    return true;
}

bool TitleImage::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Image::Create(name, id, vPos);
    
    return true;
}

bool TitleImage::UpdateTransform(float dTime) {
    Image::UpdateTransform(dTime);

    return true;
}
bool TitleImage::Update(float dTime) {

    Image::Update(dTime);
    return true;
}


bool TitleImage::Submit(float dTime)
{
    Image::Submit();
    return true;
}
