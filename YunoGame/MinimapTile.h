#pragma once
#include "Button.h"

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
class MinimapTile : public Button
{
public:
    MinimapTile(UIFactory& uiFactory);
    virtual ~MinimapTile();
    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    // 버튼 이벤트 핸들
    virtual bool IdleEvent() override;                          // 아무것도 안할 때
    virtual bool HoveredEvent() override;                       // 커서가 위에 있을 때
    virtual bool LMBPressedEvent() override;                    // 왼클릭 눌렀을 때
    virtual bool RMBPressedEvent() override;                    // 우클릭 눌렀을 때
    virtual bool KeyPressedEvent(uint32_t key = 0) override;    // 바인딩한 키 눌렀을 때
    virtual bool LMBReleasedEvent() override;                   // 왼클릭 뗐을 때
    virtual bool RMBReleasedEvent() override;                   // 우클릭 뗐을 때
    virtual bool KeyReleasedEvent(uint32_t key = 0) override;   // 바인딩한 키 뗐을 때

    virtual WidgetType GetWidgetType() override { return WidgetType::Button; }
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
