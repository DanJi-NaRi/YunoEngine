#include "pch.h"

#include "Animator.h"
#include "RenderTypes.h"
#include "UImgui.h"
#include "Parser.h"

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
    assert(boneCount <= MAX_BONE); //본 일단 넉넉하게 70개 제한 나중에 줄이기 ㄱ

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

        ChangeAnimation(curAnim);

        animCount++;
       
        return true;
    }
        
    return false;
}

bool Animator::AddAnimationFromFile(const std::string& name, const std::wstring& filepath)
{
    auto clip = Parser::Instance().LoadAnimationClipFromFile(filepath);

    AddAnimationClip(name, std::move(clip));

    return true;
}

void Animator::Update(float dTime)
{
    if (!isPlay || !m_CurAnim) return;

    //본 애니메이션 계산
    float TickTime = m_CurAnim->TickPerSec * dTime;
    CurTickTime += TickTime;

    if (CurTickTime >= m_CurAnim->duration)
    {
        if (m_CurAnim->isLoop)
            CurTickTime = 0;
        else
            isPlay = false;
    }
   
    m_RootBone->UpdateBoneMatrix(CurTickTime, m_CurAnim->channels, m_BoneTMs, XMMatrixIdentity());
}

void Animator::ChangeAnimation(UINT id)
{
    auto it = m_AnimationClips.find(id);

    if (it != m_AnimationClips.end())   
        m_CurAnim = it->second.get();
}

UINT Animator::GetCurAnimFrameCount()
{
    return 0;
}

void Animator::Serialize()
{
    if (UI::CollapsingHeader("Animator"))
    {
        if (UI::Button("Play"))
        {
            isPlay = true;
        }

        UI::SameLine();

        if (UI::Button("Stop"))
        {
            isPlay = false;
        }

        float curTimeSec = CurTickTime / m_CurAnim->TickPerSec;
        float durationSec = (float)m_CurAnim->duration / m_CurAnim->TickPerSec;

        UI::BeginDisabled(isPlay);
        if (UI::SliderFloat("TickTime", &CurTickTime, 0.0f, m_CurAnim->duration))
        {
            //CurTickTime = curTimeSec * m_CurAnim->TickPerSec;

            m_RootBone->UpdateBoneMatrix(
                CurTickTime,
                m_CurAnim->channels,
                m_BoneTMs,
                XMMatrixIdentity()
            );
        }
        if (UI::SliderFloat("RealTime", &curTimeSec, 0.0f, durationSec))
        {
            CurTickTime = curTimeSec * m_CurAnim->TickPerSec;

            m_RootBone->UpdateBoneMatrix(
                CurTickTime,
                m_CurAnim->channels,
                m_BoneTMs,
                XMMatrixIdentity()
            );
        }
        UI::EndDisabled();
    }
}
