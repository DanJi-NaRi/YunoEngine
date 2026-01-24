#pragma once
#include "Unit.h"

class Piece : public Unit
{
public:
    explicit Piece();
    virtual ~Piece();

    bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

private:
    bool CreateMesh() override;      // 메시 생성 (한 번만)
    bool CreateMaterial() override;  // 머테리얼 생성 (한 번만)

public:
    void SetTarget(XMFLOAT3 targetPos);

private:
    XMVECTOR m_Target;
    XMVECTOR m_Start;
    float m_Dist;
    float m_speed = 10;
    float m_AnimTime = 0.f;
    bool isMoving = false;
};

