#include "pch.h"

#include "Animator.h"
#include "RenderTypes.h"
#include "UImgui.h"
#include "Parser.h"

Animator::Animator()
{
    m_LocalBoneA.reserve(50);
    m_LocalBoneB.reserve(50);
    m_FinalBoneTM.reserve(50);
    animCount = 0;
    curAnim = -1;
    m_CurAnim = nullptr;
}

Animator::~Animator()
{

}

void Animator::SetBoneTree(std::unique_ptr<BoneNode>&& rootNode, const std::unordered_map<std::string, UINT>& nameToIndex, UINT boneCount)
{
    assert(boneCount <= MAX_BONE); //본 일단 넉넉하게 70개 제한 나중에 줄이기 ㄱ

    m_RootBone = std::move(rootNode);
    m_BoneNameToIndex = nameToIndex;

    m_LocalBoneA.resize(boneCount);
    m_LocalBoneB.resize(boneCount);
    m_BlendBoneTM.resize(boneCount);
    m_FinalBoneTM.resize(boneCount);
    m_GlobalBoneTM.resize(boneCount);
    m_BoneCount = boneCount;
    for (size_t i = 0; i < m_BoneCount; i++)
    {
        m_LocalBoneA[i] = XMMatrixIdentity();
        m_LocalBoneB[i] = XMMatrixIdentity();
        m_BlendBoneTM[i] = XMMatrixIdentity();
        XMStoreFloat4x4(&m_FinalBoneTM[i], XMMatrixIdentity());
    }
}

bool Animator::AddAnimationClip(const std::string& name, std::unique_ptr<AnimationClip>&& clip)
{
    auto it = m_NameToID.find(name);

    if (it == m_NameToID.end())
    {
        m_NameToID[name] = animCount;
        clip->name = name;
        m_AnimationClips.emplace(animCount, std::move(clip));
        if (curAnim == -1 || !m_CurAnim)
        {
            curAnim = animCount;
            m_CurAnim = m_AnimationClips.find(curAnim)->second.get();
        }

        animCount++;
       
        return true;
    }
        
    return false;
}

bool Animator::AddAnimationFromFile(const std::string& name, const std::wstring& filepath)
{
    auto clip = Parser::Instance().LoadAnimationClipFromFile(filepath);

    clip->name = name;
    clip->isLoop = true;
    AddAnimationClip(name, std::move(clip));

    return true;
}

void Animator::Update(float dTime)
{
    if (!isPlay || !m_CurAnim) return;

    //본 애니메이션 계산
    if (isBlending)
    {
        BlendingUpdate(dTime);
    }
    else
    {
        float TickTime = m_CurAnim->TickPerSec * dTime;
        CurTickTime += TickTime;

        if (CurTickTime >= m_CurAnim->duration)
        {
            if (m_CurAnim->isLoop)
                CurTickTime = 0;
            else
                isPlay = false;
        }

        m_RootBone->SampleLocalPose(CurTickTime, m_CurAnim->channels, m_BlendBoneTM);
    }

    m_RootBone->UpdateBoneMatrix(m_BlendBoneTM, m_FinalBoneTM, m_GlobalBoneTM, XMMatrixIdentity());
}

const XMFLOAT4X4& Animator::GetBoneGlobal(int idx)
{
    return m_GlobalBoneTM[idx];
}

int Animator::FindIndex(const std::string& name)
{
    auto it = m_BoneNameToIndex.find(name);
    if (it == m_BoneNameToIndex.end())
        return -1;

    return it->second;
}

void Animator::BlendingUpdate(float dTime)
{
    if (!m_CurAnim || !m_prevAnim) return;

    if(PrevTickTime < m_prevAnim->duration)
        PrevTickTime += m_prevAnim->TickPerSec * dTime;
    CurTickTime += m_CurAnim->TickPerSec * dTime;

    m_RootBone->SampleLocalPose(PrevTickTime, m_prevAnim->channels, m_LocalBoneA);
    m_RootBone->SampleLocalPose(CurTickTime, m_CurAnim->channels, m_LocalBoneB);

    BlendLocalPose(m_LocalBoneA, m_LocalBoneB, blendAlpha, m_BlendBoneTM);

    blendAlpha += dTime / blendDuration;

    if (blendAlpha >= 1.0f)
    {
        isBlending = false;
        blendAlpha = 0.0f;
        m_prevAnim = nullptr;
        PrevTickTime = 0;
    }
}

void Animator::BlendLocalPose(
    const std::vector<XMMATRIX>& A,
    const std::vector<XMMATRIX>& B,
    float alpha,
    std::vector<XMMATRIX>& out)
{
    for (size_t i = 0; i < A.size(); i++)
    {
        XMVECTOR s0, r0, t0;
        XMVECTOR s1, r1, t1;

        XMMatrixDecompose(&s0, &r0, &t0, A[i]);
        XMMatrixDecompose(&s1, &r1, &t1, B[i]);

        XMVECTOR s = XMVectorLerp(s0, s1, alpha);
        XMVECTOR t = XMVectorLerp(t0, t1, alpha);
        XMVECTOR r = XMQuaternionSlerp(r0, r1, alpha);

        out[i] =
            XMMatrixScalingFromVector(s) *
            XMMatrixRotationQuaternion(r) *
            XMMatrixTranslationFromVector(t);
    }
}

void Animator::SetLoop(const std::string& name, bool isLoop)
{
    auto it = m_NameToID.find(name);

    if (it != m_NameToID.end())
    {
        UINT id = m_NameToID[name];

        m_AnimationClips[id]->isLoop = isLoop;
    }
}

void Animator::SetLoop(UINT id, bool isLoop)
{
    auto it = m_AnimationClips.find(id);

    if (it != m_AnimationClips.end())
    {
        m_AnimationClips[id]->isLoop = isLoop;
    }
}


void Animator::Change(UINT id, float duration)
{
    if (id == curAnim) return;

    auto it = m_AnimationClips.find(id);
    if (it == m_AnimationClips.end())
        return;

    curAnim = id;
    m_prevAnim = m_CurAnim;
    m_CurAnim = it->second.get();
    PrevTickTime = CurTickTime;
    CurTickTime = 0;
    isBlending = true;
    blendDuration = duration;
    blendAlpha = 0.0f;
}

void Animator::Change(const std::string& name, float duration)
{
    auto it1 = m_NameToID.find(name);
    if (it1 == m_NameToID.end())
        return;

    UINT id = it1->second;

    if (id == curAnim) return;

    auto it2 = m_AnimationClips.find(id);
    if (it2 != m_AnimationClips.end())
        m_CurAnim = it2->second.get();

    curAnim = id;
    m_prevAnim = m_CurAnim;
    m_CurAnim = it2->second.get();
    PrevTickTime = CurTickTime;
    CurTickTime = 0;
    isBlending = true;
    blendDuration = duration;
    blendAlpha = 0.0f;
}

void Animator::Serialize()
{
    if (UI::CollapsingHeader("Animator"))
    {
        const char* currentName = m_CurAnim
            ? m_CurAnim->name.c_str()
            : "None";

        if (UI::BeginCombo("Animation", currentName))
        {
            for (auto& [name, clip] : m_AnimationClips)
            {
                bool isSelected = (name == curAnim);

                if (UI::Selectable(clip->name.c_str(), isSelected))
                {
                    Change(name);
                    isPlay = true;
                }

                if (isSelected)
                    UI::SetItemDefaultFocus();
            }

            UI::EndCombo();
        }

        if (UI::Button("Play"))
        {
            isPlay = true;
        }

        UI::SameLine();

        if (UI::Button("Stop"))
        {
            isPlay = false;
        }


        if (m_CurAnim)
        {
            float curTimeSec = CurTickTime / m_CurAnim->TickPerSec;
            float durationSec = (float)m_CurAnim->duration / m_CurAnim->TickPerSec;

            UI::BeginDisabled(isPlay);
            if (UI::SliderFloat("TickTime", &CurTickTime, 0.0f, m_CurAnim->duration))
            {
                //CurTickTime = curTimeSec * m_CurAnim->TickPerSec;
                m_RootBone->SampleLocalPose(CurTickTime, m_CurAnim->channels, m_BlendBoneTM);
                m_RootBone->UpdateBoneMatrix(
                    m_BlendBoneTM,
                    m_FinalBoneTM,
                    m_GlobalBoneTM,
                    XMMatrixIdentity()
                );
            }
            if (UI::SliderFloat("RealTime", &curTimeSec, 0.0f, durationSec))
            {
                CurTickTime = curTimeSec * m_CurAnim->TickPerSec;

                m_RootBone->SampleLocalPose(CurTickTime, m_CurAnim->channels, m_BlendBoneTM);
                m_RootBone->UpdateBoneMatrix(
                    m_BlendBoneTM,
                    m_FinalBoneTM,
                    m_GlobalBoneTM,
                    XMMatrixIdentity()
                );
            }
            UI::EndDisabled();
        }
    }
}
