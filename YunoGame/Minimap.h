#pragma once
#include "Image.h"
#include "Grid.h"
#include <memory>

class GridFactory;

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
protected:                             // 그리드 라인 오브젝트 생성

    ///////////////////////////////////
    // 좌표계
    //Int2 WorldToCell(float x, float z);       // 월드(x, z) -> 셀 좌표
    //Float2 CellToWorld(int cx, int cz);         // 셀 좌표 -> 월드 중심(x, z)
    //Float2 CellToUIWorld(int cx, int cz);         // 셀 좌표 -> 월드 중심(x, z)
    //bool InBounds(int cx, int cz);          // 해당 셀 좌표가 그리드 내부 좌표인지
    ///////////////////////////////////

    int GetID(int cx, int cz);
    Int2 GetCellByID(int tileID);

protected:
    // Grid // 기준 : XY
    GridXY m_pGrid;

    WidgetGridLine* m_pGridLine = nullptr;


    //WidgetGridBox* m_gridBox = nullptr;
    //std::vector<std::unique_ptr<Grid>> m_grids;

    //std::unique_ptr<GridFactory> m_gridFactory;
};

