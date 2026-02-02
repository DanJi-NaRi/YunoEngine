#include "pch.h"
#include "UnitGridBox.h"

UnitGridBox::UnitGridBox()
{
}

UnitGridBox::~UnitGridBox()
{
}

bool UnitGridBox::Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos)
{
    Unit::Create(name, id, vPos);
    Backup();

    return true;
}

bool UnitGridBox::Update(float dTime)
{
    Unit::Update(dTime);
    return true;
}

bool UnitGridBox::Submit(float dTime)
{
    Unit::Submit(dTime);
    return true;
}
