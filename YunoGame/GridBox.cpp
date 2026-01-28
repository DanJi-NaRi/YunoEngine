#include "pch.h"
#include "GridBox.h"

GridBox::GridBox()
{
}

GridBox::~GridBox()
{
}

bool GridBox::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Unit::Create(name, id, vPos);
    Backup();

    return true;
}

bool GridBox::Update(float dTime)
{
    Unit::Update(dTime);
    return true;
}

bool GridBox::Submit(float dTime)
{
    Unit::Submit(dTime);
    return true;
}
