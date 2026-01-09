#pragma once

#include "Unit.h"

// 각 오브젝트 헤더 파일 최상단에는 Unit 헤더가 있어야됨
class Quad : public Unit
{
public:
    explicit Quad();
    virtual ~Quad();

    bool Create(XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;
private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

    //추가 머테리얼
    MaterialHandle m_addMaterial = 0;
};

// 주석도
// 게임 , 엔진
// 게임쪽 >> 사용자가 어떻게 쓰는지 
// 엔진쪽 >> 어느 흐름으로 흘러가는지 