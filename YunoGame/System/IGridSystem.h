#pragma once

#include "YunoTransform.h"

class IGridSystem
{
public:
    virtual ~IGridSystem() = default;

    virtual void CreateObject(float x, float y, float z) = 0;                       // 그리드 기반 오브젝트 생성
    virtual void Update(float dt) = 0;

protected:
    virtual void CreateGridBox(float x, float y, float z) = 0;                      // 그리드 내부 모든 오브젝트들을 자식으로 삼는 부모 오브젝트 생성
    virtual void CreateGrid(int row, int column, float cellSizeX, float cellSizeZ) = 0;

    virtual int GetID(int cx, int cz) = 0;                                          // 셀 좌표 -> 셀 아이디
    virtual Int2 GetCellByID(int tileID) = 0;                                         // 셀 아이디 -> 셀 좌표
};
