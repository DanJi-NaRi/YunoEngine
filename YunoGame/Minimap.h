#pragma once
#include "PhasePanel.h"
#include "Grid.h"
#include <memory>


//디버그
class WidgetGridLine;
class MinimapTile;
class CardConfirmArea;

class Minimap : public PhasePanel
{
public:
    // Widget

    Minimap(UIFactory& uiFactory);
    virtual ~Minimap();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    void CreateChild() override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
    void Clear();

    virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::Minimap; }

    virtual bool CreateMaterial() override { return Widget::CreateMaterial(L"../Assets/UI/PLAY/PhaseScene/window_popup_minimap.png"); };  // 머테리얼 생성


    void SetupPanel();

    void UpdatePanel(const BattleResult& battleResult) override;
    void UpdatePanel(const ObstacleResult& obstacleResult) override;

    void StartDirChoice(CardConfirmArea* CardSlot);
    void SetButtonLock(bool buttonLock); // SetButtonLock

    void OpenDirButton(int tileID, CardConfirmArea* CardSlot);

    bool GetButtonLock() { return m_buttonLock; } // SetButtonLock
    
    int GetTileID(int cx, int cy) const;
    int GetTileID(Int2 tileXY) const;
    Int2 GetCellByID(int tileID) const;
    MinimapTile* GetTileByID(int tileID);
    MinimapTile* GetTileByID(Int2 tileXY);
    bool IsValidTileID(int tileID) const;
    bool IsValidTileID(Int2 tileXY) const;
    

protected:                             // 그리드 라인 오브젝트 생성

    void SetupGrid(); // 기본 그리드 셋업 (타일 생성 등..)
    void ClearGrid();

protected:
    bool m_buttonLock = true;
    // Grid // 기준 : XY
    GridXY m_grid;
    std::vector<MinimapTile*> m_pTiles;
    WidgetGridLine* m_pGridLine = nullptr;
};

