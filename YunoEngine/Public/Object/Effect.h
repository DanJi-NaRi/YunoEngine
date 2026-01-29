#pragma once
#include "Unit.h"
#include "EffectTemplate.h"

//쿼드 한 개 스프라이트 이펙트
class Effect : public Unit
{
public:
    bool m_active = false;

    float m_lifetime = 1.0f;
    float m_age = 0.0f;

    int m_frameCount = 16;
    int m_cols = 4;
    int m_rows = 4;

    bool flipbook = true;

    bool m_loop = false;
    BillboardMode billboard;
public:
    Effect();
    virtual ~Effect() = default;

    virtual void SetTemplate(const EffectTemplate& temp);
    virtual void Play(const XMFLOAT3& pos, const XMFLOAT3& scale, const XMFLOAT3& dir);
    virtual void Reset();

    virtual void UpdateWorldMatrix();
    virtual void UpdateFrame();

    XMMATRIX UpdateBillBoard();

    XMMATRIX UpdateDefault();
    XMMATRIX UpdateScreenAlign();
    XMMATRIX UpdateWorldUpAlign();
    XMMATRIX UpdateAxisLock();

    virtual bool Update(float dt) override;
    virtual bool Submit(float dt) override;
};
