#include "pch.h"
#include "TextureImage.h"

TextureImage::TextureImage(UIFactory& uiFactory)
    : Image(uiFactory)
{
}

TextureImage::~TextureImage()
{
}

bool TextureImage::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);
    return true;
}

bool TextureImage::Update(float dTime)
{
    Image::Update(dTime);
    return true;
}

bool TextureImage::Submit(float dTime)
{
    Image::Submit(dTime);
    return true;
}
