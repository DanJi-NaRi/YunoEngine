#pragma once

struct I2
{
    int x, z;
};
struct F2
{
    float x, z;
};

class IGridSystem
{
public:
    virtual ~IGridSystem() = default;

    virtual void Init(int row, int column, float cellSizeX, float cellSizeZ) = 0;   // 그리드 정보 초기화
    virtual void CreateObject(float x, float y, float z) = 0;                       // 그리드 기반 오브젝트 생성
    virtual void Update(float dt) = 0;

protected:
    virtual bool InBounds(int cx, int cz) = 0;
    virtual I2 WorldToCell(float x, float z) = 0;                                   // 월드(x, z) -> 셀 좌표
    virtual F2 CellToWorld(int cx, int cz) = 0;                                     // 셀 좌표 -> 월드 중심(x, z)

    virtual int GetID(int cx, int cz) = 0;                                          // 셀 좌표 -> 셀 아이디
};
