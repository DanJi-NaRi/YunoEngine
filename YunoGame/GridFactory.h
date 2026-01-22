#pragma once

class GridFactory
{
public:
    GridFactory();
    ~GridFactory();
    void Init(int row, int column, float cellSizeX, float cellSizeZ);
    static void GetGridInfo(int& row, int& column, float& cellSizeX, float& cellSizeZ);

private:
    static int m_row, m_column;
    static float m_cellSizeX, m_cellSizeZ;
};
