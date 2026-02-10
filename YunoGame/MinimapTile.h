#pragma once
#include "Image.h"

enum class TileTeamData : uint8_t {
    Default,
    Red,  // Team1
    Blue, // Team2
};

struct TileData {
    bool isPlayerTile; // 플레이어가 있는지
    uint8_t teamID;
    uint8_t unitID;
     // 타일 아이디
};
class MinimapTile : public Image
{
public:
    MinimapTile(UIFactory& uiFactory);
    virtual ~MinimapTile();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::MinimapTile; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/map_cube_non.png"); };

    TileData& GetTileData() { return m_data; }
    bool IsPlayerTile() { return m_data.isPlayerTile; }
    void SetTileId(uint8_t tileID) { m_tileID = tileID; };
    void SetTileData(TileData& tile) { m_data = tile; }
    const int& GetTileId() { return m_tileID; };

    void ClearTileData();

protected:
    uint8_t m_tileID;
    TileData m_data;

    bool isSimulation; // 시뮬레이션 중인지 판단
};
