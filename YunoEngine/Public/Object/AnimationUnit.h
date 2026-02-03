#pragma once
#include "Unit.h"
#include "Animator.h"

class AnimationUnit : public Unit
{
protected:
    std::unique_ptr<Animator> m_animator;

    std::unordered_map<UINT, std::vector<Unit*>> m_BoneChilds; //본에 붙는 자식들
    std::unordered_map<Unit*, UINT> m_ChildToIdx; //본에 붙는 자식들
public:
    AnimationUnit();
    virtual ~AnimationUnit() = default;

    virtual void AnimationUpdate(float dTime);
    virtual bool Update(float dTime) override; 
    virtual bool  Submit(float dTime = 0) override;

    void SetAnimator(std::unique_ptr<Animator>&& animator) { m_animator = std::move(animator); }
    void AddAnimationClip(const std::string& name, const std::wstring& filepath);

    void AttachChildBone(Unit* child, int idx);
    void AttachChildBone(Unit* child, const std::string& bonename);

    virtual void DettachChild(uint32_t id) override;
    virtual void ClearChild() override;

    XMMATRIX GetWorldBoneMatrix();

    void SetLoop(const std::string& name, bool isLoop);
    void SetLoop(UINT id, bool isLoop);

#ifdef _DEBUG
    virtual void Serialize() override;
#endif
};
