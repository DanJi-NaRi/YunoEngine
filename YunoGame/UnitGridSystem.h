#pragma once
#include "IGridSystem.h"

class ObjectManager;
class Unit;
class GridFactory;
class Grid;

class UnitGridBox;

class UnitGridSystem : public IGridSystem
{
protected:
    ObjectManager* m_manager = nullptr;

public:
    UnitGridSystem(ObjectManager* manager);                                                 // 씬에서 manager 받기
    virtual ~UnitGridSystem();

    void CreateObject(float x, float y, float z) = 0;
    void Update(float dt) = 0;

protected:
    void CreateGrid(int row, int column, float cellSizeX, float cellSizeZ) override;
    void CreateGridBox(float x, float y, float z);
    void CreateGridLine(float x, float y, float z);                         // 그리드 라인 오브젝트 생성

    void SetNG_P(Grid* NG_P);
    int GetID(int cx, int cz) override;
    I2 GetCellByID(int tileID) override;

protected:
    int m_row, m_column;
    int m_halfCellsX, m_halfCellsZ;
    float m_cellSizeX, m_cellSizeZ;
    float m_invX, m_invZ;

protected:
    UnitGridBox* m_gridBox = nullptr;
    std::vector<std::unique_ptr<Grid>> m_grids;

    std::unique_ptr<GridFactory> m_gridFactory;
};
