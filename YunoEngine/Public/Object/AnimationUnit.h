#pragma once
#include "Unit.h"
#include "Animator.h"

class AnimationUnit : public Unit
{
protected:
    std::unique_ptr<Animator> m_animator;
public:
    virtual void AnimationUpdate(float dTime);
    virtual bool Update(float dTime) override; 
    virtual bool  Submit(float dTime = 0) override;

    void SetAnimator(std::unique_ptr<Animator>&& animator) { m_animator = std::move(animator); }
    void AddAnimationClip(const std::string& name, const std::wstring& filepath);

    virtual void Serialize() override;
};
