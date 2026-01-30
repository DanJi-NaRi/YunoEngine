#pragma once

#include "SimpleMath.h"

template<typename T>
class Grid
{
public:
    Grid(int row, int column, float cellSizeX, float cellSizeZ);

    I2 WorldToCell(float x, float z);       // 월드(x, z) -> 셀 좌표
    F2 CellToWorld(int cx, int cz);         // 셀 좌표 -> 월드 중심(x, z)
    bool InBounds(int cx, int cz);          // 해당 셀 좌표가 그리드 내부 좌표인지

public:
    int m_row, m_column;
    int m_halfCellsX, m_halfCellsZ;
    float m_cellSizeX, m_cellSizeZ;
    float m_invX, m_invZ;
};

class Unit;
class Widget;

extern template class Grid<Unit>;
extern template class Grid<Widget>;


template<>
F2 Grid<Widget>::CellToWorld(int cx, int cz);
