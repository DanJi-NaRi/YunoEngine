#include "pch.h"
#include "GridFactory.h"

GridFactory::GridFactory()
{
}

GridFactory::~GridFactory()
{
}

void GridFactory::Init(int row, int column)
{
    m_row = row;
    m_column = column;
}

void GridFactory::GetGridInfo(int& row, int& column)
{
    row = m_row;
    column = m_column;
}

int GridFactory::m_row = 5;
int GridFactory::m_column = 7;
