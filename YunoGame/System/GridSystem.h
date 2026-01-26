#pragma once
#include "IGridSystem.h"

class ObjectManager;
class GridFactory;

class GridSystem : public IGridSystem
{
protected:
    ObjectManager* m_objectManager = nullptr;

public:
    GridSystem(ObjectManager* objmng);                                      // 씬에서 objmanager 받기
    virtual ~GridSystem();

    void Init(int row, int column, float cellSizeX, float cellSizeZ) override;  
    void CreateObject(float x, float y, float z) = 0;
    void Update(float dt) = 0;
    
    void CreateGridLine(float x, float y, float z);                         // 그리드 라인 오브젝트 생성

protected:
    bool InBounds(int cx, int cz) override;
    I2 WorldToCell(float x, float z) override;
    F2 CellToWorld(int cx, int cz) override;

    int GetID(int cx, int cz) override;

protected:
    int m_row, m_column;
    int m_halfCellsX, m_halfCellsZ;
    float m_cellSizeX, m_cellSizeZ;
    float m_invX, m_invZ;
    
protected:

    std::unique_ptr<GridFactory> m_gridFactory;
};

