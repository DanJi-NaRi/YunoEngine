#include "pch.h"
#include "MinimapTile.h"

#include "YunoEngine.h"
#include "IInput.h"

MinimapTile::MinimapTile(UIFactory& uiFactory) : Button(uiFactory) // 오른쪽에 부모의 생성자를 반드시 호출해줄 것.
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
    Button::Create(name, id, sizePx, vPos, rotZ, vScale);

    m_backUpOriginPath = m_texturePathBk;

    return true;
}

void MinimapTile::CreateChild()
{
}

bool MinimapTile::Update(float dTime) {

    if (!IsUseLMB()) {
        switch ((TileTeamData)m_data.teamID) {
        default:
        case TileTeamData::Default: ChangeTexture(g_tilePath_None); break;
        case TileTeamData::Red: ChangeTexture(g_tilePath_Red); break;
        case TileTeamData::Blue: ChangeTexture(g_tilePath_Blue); break;
        }
    }


    Button::Update(dTime);

    return true;
}

void MinimapTile::DefaultMinimapSetup() {
    ResetHoverTexture(m_backUpOriginPath);
    SetUseLMB(false);
    SetUseRMB(false);
    SetRotZ(0);
    MirrorReset();
    SetEventLMB(nullptr);
    SetEventRMB(nullptr);
}


bool MinimapTile::Submit(float dTime)
{
    Button::Submit();
    return true;
}

// 가만히 있을 때
bool MinimapTile::IdleEvent()
{
    //std::cout << "IdleEvent" << std::endl;
    // 내용 작성
    return true;
}

// 커서가 위에 올라있을 때
bool MinimapTile::HoveredEvent()
{

   //std::cout << "HoveredEvent" << std::endl;

    return true;
}

// Down 기능은 현재 미지원
//bool MinimapTile::DownEvent()
//{
//    std::cout << "DownEvent" << std::endl;
//    return true;
//}

// 왼클릭 눌렀을 때
bool MinimapTile::LMBPressedEvent()
{
    std::cout << "(LMB)PressedEvent - " << static_cast<int>(m_tileID) << std::endl;
    return true;
}

// 우클릭 눌렀을 때
bool MinimapTile::RMBPressedEvent()
{
    std::cout << "(RMB)PressedEvent - " << static_cast<int>(m_tileID) << std::endl;
    return true;
}

// 바인딩한 키 눌렀을 때
bool MinimapTile::KeyPressedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)PressedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')PressedEvent" << std::endl;
    return true;
}

// 왼클릭 뗐을 때
bool MinimapTile::LMBReleasedEvent()
{
    std::cout << "(LMB)ReleasedEvent - " << static_cast<int>(m_tileID) << std::endl;
    return true;
}

// 우클릭 뗐을 때
bool MinimapTile::RMBReleasedEvent()
{
    std::cout << "(RMB)ReleasedEvent - " << static_cast<int>(m_tileID) << std::endl;
    return true;
}

// 바인딩한 키 뗐을 때
bool MinimapTile::KeyReleasedEvent(uint32_t key)
{
    if (key == 0) std::cout << "(Key)ReleasedEvent" << std::endl;
    else std::cout << "(Key - " << key << ", \'" << static_cast<char>(key) << "\')ReleasedEvent" << std::endl;
    return true;
}
