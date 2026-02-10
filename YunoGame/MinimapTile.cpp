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

void MinimapTile::ClearTileData()
{
    m_data.isPlayerTile = false;
    m_data.teamID = (uint8_t)TileTeamData::Default;
    m_data.unitID = 0;
}

bool MinimapTile::Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale)
{
    Image::Create(name, id, sizePx, vPos, rotZ, vScale);

    return true;
}

bool MinimapTile::Update(float dTime) {

    switch ((TileTeamData)m_data.teamID) {
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
