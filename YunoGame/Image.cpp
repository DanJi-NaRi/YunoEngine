#include "pch.h"
#include "Image.h"


Image::Image()
{
}

Image::~Image()
{
}

bool Image::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Widget::Create(name, id, vPos);

    GetDefWidgetMesh(m_defaultMesh); // 기본 quad 적용
    if(!m_defaultMesh)return false;

    return true;
}

bool Image::Update(float dTime)
{
    Widget::Update(dTime);
    return true;
}
