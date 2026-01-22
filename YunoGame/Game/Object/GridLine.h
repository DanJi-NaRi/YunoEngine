#pragma once
#include "Unit.h"

class GridLine : public Unit
{
public:
    explicit GridLine();
    virtual ~GridLine();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

private:
    // 라인 그리기용 버텍스
    std::vector<VERTEX_Pos> m_lineVtx;
};

