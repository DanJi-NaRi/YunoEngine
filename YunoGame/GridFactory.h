#pragma once

class GridFactory
{
public:
    GridFactory();
    ~GridFactory();
    void Init(int row, int column);
    static void GetGridInfo(int& row, int& column);

private:
    static int m_row, m_column;
};
