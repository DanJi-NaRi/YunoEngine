#pragma once
#include "IGridSystem.h"

class ObjectManager;
class UIManager;
class Unit;
class Widget;
class GridFactory;
template<typename T>
class Grid;


template<typename T>
class GridBox;


template<typename T, typename R>
class GridSystem : public IGridSystem
{
protected:
    T* m_manager = nullptr;

public:
    GridSystem(T* manager);                                                 // 씬에서 manager 받기
    virtual ~GridSystem();

    void CreateObject(float x, float y, float z) = 0;
    void Update(float dt) = 0;
    
protected:
    void CreateGrid(int row, int column, float cellSizeX, float cellSizeZ) override;
    void CreateGridBox(float x, float y, float z);
    void CreateGridLine(float x, float y, float z);                         // 그리드 라인 오브젝트 생성

    void SetNG_P(Grid<R>* NG_P);
    int GetID(int cx, int cz) override;
    I2 GetCellByID(int tileID) override;

protected:
    int m_row, m_column;
    int m_halfCellsX, m_halfCellsZ;
    float m_cellSizeX, m_cellSizeZ;
    float m_invX, m_invZ;
    
protected:
    GridBox<R>* m_gridBox = nullptr;
    Grid<R>* m_grids;

    std::unique_ptr<GridFactory> m_gridFactory;
};

extern template class GridSystem<ObjectManager, Unit>;
extern template class GridSystem<UIManager, Widget>;

template<>
void GridSystem<UIManager, Widget>::CreateGridLine(float x, float y, float z);

