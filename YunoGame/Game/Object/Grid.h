#pragma once

#include "YunoTransform.h"

class Grid
{
public:
    Grid(int row, int column, float cellSizeX, float cellSizeZ);

    Int2 WorldToCell(float x, float z);       // 월드(x, z) -> 셀 좌표
    Float2 CellToWorld(int cx, int cz);         // 셀 좌표 -> 월드 중심(x, z)
    Float2 CellToUIWorld(int cx, int cz);         // 셀 좌표 -> 월드 중심(x, z)
    bool InBounds(int cx, int cz);          // 해당 셀 좌표가 그리드 내부 좌표인지

public:
    int m_row, m_column;
    int m_halfCellsX, m_halfCellsZ;
    float m_cellSizeX, m_cellSizeZ;
    float m_invX, m_invZ;
};

struct GridXY
{
    GridXY(int row, int column, float cellSizeX, float cellSizeZ);

    Int2 WorldToCell(float x, float y);       // 월드(x, z) -> 셀 좌표
    Float2 CellToWorld(int cx, int cy);         // 셀 좌표 -> 월드 중심(x, y)
    Float2 CellToUIWorld(int cx, int cy);         // 셀 좌표 -> 월드 중심(x, y)
    bool InBounds(int cx, int cy);          // 해당 셀 좌표가 그리드 내부 좌표인지

    int row, column;
    Int2 halfCells;
    Float2 cellSize;
    Float2 inv;
};
