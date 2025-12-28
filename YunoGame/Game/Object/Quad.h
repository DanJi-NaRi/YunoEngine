#pragma once

#include "Unit.h"

class Quad : public Unit
{
public:
    explicit Quad();
    virtual ~Quad();

    bool Create(XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;

private:
    bool CreateQuadMesh();      // 메시 생성 (한 번만)
    bool CreateQuadMaterial();  // 머테리얼 생성 (한 번만)
};

