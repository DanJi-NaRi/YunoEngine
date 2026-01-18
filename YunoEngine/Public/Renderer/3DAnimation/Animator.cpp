#include "pch.h"

#include "Animator.h"
#include "RenderTypes.h"

Animator::Animator()
{
    m_BoneTMs.reserve(50);
    animCount = 0;
    curAnim = -1;
}

Animator::~Animator()
{

}

void Animator::SetBoneTree(std::unique_ptr<BoneNode>&& rootNode, UINT boneCount)
{
    assert(boneCount <= MAX_BONE); //본 일단 넉넉하게 40개 제한 나중에 줄이기 ㄱ

    m_RootBone = std::move(rootNode);

    m_BoneTMs.resize(boneCount);
    for (size_t i = 0; i < m_BoneTMs.size(); i++)
    {
        XMStoreFloat4x4(&m_BoneTMs[i], XMMatrixIdentity());
    }
}

bool Animator::AddAnimationClip(const std::string& name, std::unique_ptr<AnimationClip>&& clip)
{
    auto it = m_NameToID.find(name);

    if (it == m_NameToID.end())
    {
        m_NameToID[name] = animCount;
        m_AnimationClips.emplace(animCount, std::move(clip));
        if (curAnim == -1) curAnim = animCount;

        animCount++;
       
        return true;
    }
        
    return false;
}

void Animator::Update(float dTime)
{
    if (!isPlay) return;

    auto it = m_AnimationClips.find(curAnim);
    if (it == m_AnimationClips.end())
        return;

    auto& curClip = it->second;

    //본 애니메이션 계산
    float TickTime = curClip->TickPerSec * dTime;
    CurTickTime += TickTime;

    if (CurTickTime >= curClip->duration)
    {
        if (curClip->isLoop)
            CurTickTime = 0;
        else
            isPlay = false;
    }
   
    m_RootBone->UpdateBoneMatrix(CurTickTime, it->second->channels, m_BoneTMs, XMMatrixIdentity());
}
