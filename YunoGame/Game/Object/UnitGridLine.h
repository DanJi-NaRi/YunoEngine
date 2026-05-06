#pragma once
#include "Unit.h"
#include "GridFactory.h"

class UnitGridLine : public Unit
{
public:
    UnitGridLine();
    virtual ~UnitGridLine();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos);
    bool Update(float dTime = 0);
    bool Submit(float dTime = 0);
private:
    bool CreateMesh();      // 메시 생성 (한 번만)
    bool CreateMaterial();  // 머테리얼 생성 (한 번만)

private:
    // 라인 그리기용 버텍스
    std::vector<VERTEX_Pos> m_lineVtx;
};
