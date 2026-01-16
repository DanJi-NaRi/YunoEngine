#include "pch.h"

#include "Animator.h"

Animator::Animator()
{
    m_BoneTMs.reserve(50);
    animCount = 0;
}

Animator::~Animator()
{

}

void Animator::SetBoneTree(std::unique_ptr<BoneNode>&& rootNode, UINT boneCount)
{
    m_RootBone = std::move(rootNode);

    m_BoneTMs.resize(boneCount);
}

bool Animator::AddAnimationClip(const std::string& name, std::unique_ptr<AnimationClip>&& clip)
{
    auto it = m_NameToID.find(name);

    if (it == m_NameToID.end())
    {
        m_NameToID[name] = animCount;
        m_AnimationClips.emplace(animCount, std::move(clip));

        animCount++;
        return true;
    }
        
    return false;
}

void Animator::Update(float dTime)
{
    //본 애니메이션 계산
    UINT TickTime = TickPerSec * dTime;

    auto it = m_AnimationClips.find(curAnim);
    if (it == m_AnimationClips.end())
        return;

    m_RootBone->UpdateBoneMatrix(TickTime, it->second->channels, m_BoneTMs, XMMatrixIdentity());
}
