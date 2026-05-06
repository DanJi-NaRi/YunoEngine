#pragma once
#include "AnimationUnit.h"

class AnimTest : public AnimationUnit
{
public:
    explicit AnimTest();
    virtual ~AnimTest();

    void SetDissolveColor(const XMFLOAT3& col);
    void SetNoiseTexture(const std::wstring& filepath);

    virtual bool Update(float dTime) override;
    void UpdateDissolve(float dt);

#ifdef _DEBUG
    virtual void Serialize() override;
#endif

protected:
    float m_dissolveAmount = 0;
    float m_dissolveWidth = 0.0f;
    XMFLOAT3 m_dissolveColor = { 1, 1, 1 };
};
