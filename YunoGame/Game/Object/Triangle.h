#pragma once

#include "Unit.h"

class Triangle : public Unit
{
public:
    explicit Triangle();
    virtual ~Triangle();

    bool Create(XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial()override;  // 머테리얼 생성 (한 번만)

};

