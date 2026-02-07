#include "pch.h"
#include "MinimapTile.h"

#include "YunoEngine.h"
#include "IInput.h"

MinimapTile::MinimapTile(UIFactory& uiFactory) : Image(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
{
    Clear(); // Clear 추가는 기본적으로!!
}

MinimapTile::~MinimapTile()
{
    Clear(); // Clear 추가는 기본적으로!!
}

void MinimapTile::Clear()
{
}

bool MinimapTile::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);

    return true;
}

bool MinimapTile::Update(float dTime) {
    Image::Update(dTime);


    return true;
}


bool MinimapTile::Submit(float dTime)
{
    Image::Submit();
    return true;
}
