#pragma once
#include "Image.h"

enum class TileTeamData {
    Default,
    Red,  // Team1
    Blue, // Team2
};

struct TileData {
    bool isPlayerTile; // 플레이어가 있는지
    TileTeamData m_teamId;
    int tileId; // 타일 아이디
    int unitId;
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
    TileData& GetTileData() { return m_data; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/map_cube_non.png"); };
    bool IsPlayerTile() { return m_data.isPlayerTile; }

protected:
    bool isSimulation; // 시뮬레이션 중인지 판단
    TileData m_data;
};
