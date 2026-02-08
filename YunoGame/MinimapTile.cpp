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

    if(YunoEngine::GetInput()->IsKeyDown('X')) {
        m_data.m_teamId = TileTeamData::Default;
    }
    if (YunoEngine::GetInput()->IsKeyDown('C')) {
        m_data.m_teamId = TileTeamData::Red;
    }
    if (YunoEngine::GetInput()->IsKeyDown('V')) {
        m_data.m_teamId = TileTeamData::Blue;
    }
    switch (m_data.m_teamId) {
    default:
    case TileTeamData::Default: ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/map_cube_non.png"); break;
    case TileTeamData::Red: ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/map_cube_red.png"); break;
    case TileTeamData::Blue: ChangeTexture(L"../Assets/UI/PLAY/PhaseScene/map_cube_blue.png"); break;
    }

    Image::Update(dTime);

    return true;
}


bool MinimapTile::Submit(float dTime)
{
    Image::Submit();
    return true;
}
