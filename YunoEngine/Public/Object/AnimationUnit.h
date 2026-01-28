#pragma once
#include "Unit.h"
#include "Animator.h"

class AnimationUnit : public Unit
{
protected:
    std::unique_ptr<Animator> m_animator;
public:
    AnimationUnit();
    virtual ~AnimationUnit() = default;

    virtual void AnimationUpdate(float dTime);
    virtual bool Update(float dTime) override; 
    virtual bool  Submit(float dTime = 0) override;

    void SetAnimator(std::unique_ptr<Animator>&& animator) { m_animator = std::move(animator); }
    void AddAnimationClip(const std::string& name, const std::wstring& filepath);

#ifdef _DEBUG
    virtual void Serialize() override;
#endif
};
