#pragma once

#include "Effect.h"

// 풀링 없이 ObjectManager에서 일반 Unit처럼 관리하는 이펙트 오브젝트
class EffectUnit : public Effect
{
public:
    EffectUnit();
    virtual ~EffectUnit() = default;

    virtual bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos) override;
    virtual bool Create(const std::wstring& name, uint32_t id, XMFLOAT3 vPos, PassOption opt) override;

    virtual bool Update(float dt) override;

    // 텍스처를 지정해서 이펙트 머티리얼을 내부 생성
    bool BuildInternalEffectMaterial(const EffectDesc& desc);

    // 수동 정지(즉시 초기화)
    void Stop();

#ifdef _DEBUG
    void Serialize() override;
#endif
};
