#include "pch.h"

#include "Animator.h"

Animator::Animator()
{
    m_BoneTMs.reserve(50);
}

Animator::~Animator()
{

}

void Animator::SetBoneTree(std::unique_ptr<BoneNode> rootNode, UINT boneCount)
{
    m_RootBone = std::move(rootNode);

    m_BoneTMs.resize(boneCount);
}

bool Animator::AddAnimationClip(unsigned int id, const std::unordered_map<UINT, std::unique_ptr<BoneAnimation>>& clip)
{
    auto it = m_AnimationClips.find(id);

    if (it == m_AnimationClips.end())
    {
        m_AnimationClips.emplace(id, clip);
        return true;
    }
        
    return false;
}

void Animator::Update(float dTime)
{
    //본 애니메이션 계산
    m_RootBone->UpdateBoneMatrix(dTime, m_AnimationClips[curAnim], m_BoneTMs, XMMatrixIdentity());
}
