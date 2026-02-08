#pragma once
#include "Image.h"
#include "Grid.h"
#include <memory>

class GridFactory;
class MinimapTile;

struct ObstacleResult;

//디버그
class WidgetGridLine;



class Minimap : public Image
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

    void UpdateMinimap(const ObstacleResult& obstacleResult);

protected:                             // 그리드 라인 오브젝트 생성

   

    void GridSetup(); // 기본 그리드 셋업 (타일 생성 등..)

    int GetID(int cx, int cz);
    Int2 GetCellByID(int tileID);

    

protected:
    // Grid // 기준 : XY
    GridXY m_grid;
    std::vector<MinimapTile*> m_pTiles;
    WidgetGridLine* m_pGridLine = nullptr;

    //ObstacleResult* m_pUnitData = nullptr; // 모든 유닛 정보 담겨있는 패킷

    //std::vector<TileState> m_tiles;                         // 타일 상태
    //std::vector<uint32_t> m_tilesIDs;                       // 타일 오브젝트 아이디
    //std::unordered_map<GamePiece, PieceInfo> m_pieces;      // 기물 정보


    //WidgetGridBox* m_gridBox = nullptr;
    //std::vector<std::unique_ptr<Grid>> m_grids;

    //std::unique_ptr<GridFactory> m_gridFactory;
};

