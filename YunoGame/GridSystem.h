#pragma once
#include "GridFactory.h"

class ObjectManager;

enum class CellOccupancy : uint8_t
{
    Collapesed,
    Unoccupied,
    Ally_Occupied,
    Enemy_Occupied
};

enum class CellEvent : uint8_t
{
    None,
    Heal,
    Lazer,
    Fall
};

struct CellState
{
    CellOccupancy co;
    CellEvent ce;
    CellState() : co(CellOccupancy::Unoccupied), ce(CellEvent::None) {}
};

class GridSystem
{
public:
    GridSystem(ObjectManager* objmng);  // 씬에서 objmanager 받기
    ~GridSystem();
    void Init(int row, int column, float cellSizeX, float cellSizeZ);
    void CreateObject(float x, float y, float z);
    void ClearCellState();

    int GetID(int cx, int cz);
    bool InBounds(int cx, int cz);

    // 월드(x, z) -> 셀 좌표
    std::pair<int, int> WorldToCell(float x, float z);
    // 셀 좌표 -> 월드 중심(x, z)
    std::pair<float, float> CellToWorld(int cx, int cz);
    
private:

    bool tst = true;
private:
    int m_row, m_column;
    int m_halfCellsX, m_halfCellsZ;
    float m_cellSizeX, m_cellSizeZ;
    float m_invX, m_invZ;

    GridFactory m_gridFactory;

    std::vector<CellState> m_cells;
    std::vector<uint32_t> m_tileIDs;

    ObjectManager* m_objectManager = nullptr;
};

