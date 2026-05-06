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
    XMStoreFloat4x4(&m_Identity, XMMatrixIdentity());
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
    m_GlobalBoneNoOffsetTM.resize(boneCount);
    m_BoneCount = boneCount;
    for (size_t i = 0; i < m_BoneCount; i++)
    {
        m_LocalBoneA[i] = XMMatrixIdentity();
        m_LocalBoneB[i] = XMMatrixIdentity();
        m_BlendBoneTM[i] = XMMatrixIdentity();
        XMStoreFloat4x4(&m_FinalBoneTM[i], XMMatrixIdentity());
        XMStoreFloat4x4(&m_GlobalBoneNoOffsetTM[i], XMMatrixIdentity());
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

void Animator::DispatchFrameEvents(UINT clipID, float prevTickTime, float curTickTime, bool looped)
{
    auto clipIt = m_AnimationClips.find(clipID);
    if (clipIt == m_AnimationClips.end()) return;

    auto eventIt = m_FrameEvents.find(clipID);
    if (eventIt == m_FrameEvents.end()) return;

    auto dispatchInRange = [&](UINT beginFrame, UINT endFrame)
        {
            for (UINT frame = beginFrame; frame <= endFrame; ++frame)
            {
                auto frameIt = eventIt->second.find(frame);
                if (frameIt == eventIt->second.end())
                    continue;

                for (auto& callback : frameIt->second)
                {
                    if (callback)
                        callback();
                }
            }
        };

    const UINT prevFrame = static_cast<UINT>(prevTickTime);
    const UINT curFrame = static_cast<UINT>(curTickTime);
    const UINT maxFrame = clipIt->second->duration > 0 ? clipIt->second->duration - 1 : 0;

    if (looped)
    {
        if (prevFrame < maxFrame)
            dispatchInRange(prevFrame + 1, maxFrame);
        if (curFrame >= 0)
            dispatchInRange(0, curFrame);
    }
    else
    {
        if (curFrame > prevFrame)
            dispatchInRange(prevFrame + 1, curFrame);
    }
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
        const float prevTickTime = CurTickTime;
        const float tickTime = m_CurAnim->TickPerSec * dTime;
        CurTickTime += tickTime;

        bool looped = false;
        if (CurTickTime >= m_CurAnim->duration)
        {
            if (m_CurAnim->isLoop)
            {
                CurTickTime = 0;
                looped = true;
            }
            else
            {
                CurTickTime = static_cast<float>(m_CurAnim->duration);
                isPlay = false;
            }
        }

        DispatchFrameEvents(curAnim, prevTickTime, CurTickTime, looped);

        m_RootBone->SampleLocalPose(CurTickTime, m_CurAnim->channels, m_BlendBoneTM);
    }

    m_RootBone->UpdateBoneMatrix(
        m_BlendBoneTM,
        m_FinalBoneTM,
        m_GlobalBoneTM,
        m_GlobalBoneNoOffsetTM,
        XMMatrixIdentity()
    );
}

const XMFLOAT4X4& Animator::GetBoneGlobal(int idx)
{
    if (idx < 0 || idx >= m_BoneCount)
        return m_Identity;

    return m_GlobalBoneTM[idx];
}

const XMFLOAT4X4& Animator::GetBoneGlobalNoOffset(int idx)
{
    if (idx < 0 || idx >= m_BoneCount)
        return m_Identity;

    return m_GlobalBoneNoOffsetTM[idx];
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
    //m_RootBone->SampleLocalPose(0, m_CurAnim->channels, m_LocalBoneB);

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

bool Animator::SetLoop(const std::string& name, bool isLoop)
{
    auto it = m_NameToID.find(name);

    if (it != m_NameToID.end())
    {
        UINT id = m_NameToID[name];

        m_AnimationClips[id]->isLoop = isLoop;

        return true;
    }
    return false;
}

bool Animator::SetLoop(UINT id, bool isLoop)
{
    auto it = m_AnimationClips.find(id);

    if (it != m_AnimationClips.end())
    {
        m_AnimationClips[id]->isLoop = isLoop;

        return true;
    }
    return false;
}


bool Animator::Change(UINT id, float duration)
{
    if (id == curAnim) return false;

    auto it = m_AnimationClips.find(id);
    if (it == m_AnimationClips.end())
        return false;

    curAnim = id;
    m_prevAnim = m_CurAnim;
    m_CurAnim = it->second.get();
    PrevTickTime = CurTickTime;
    CurTickTime = 0;
    isBlending = true;
    blendDuration = duration;
    blendAlpha = 0.0f;

    return true;
}

bool Animator::Change(const std::string& name, float duration)
{
    auto it1 = m_NameToID.find(name);
    if (it1 == m_NameToID.end())
        return false;

    UINT id = it1->second;

    if (id == curAnim) return false;

    auto it2 = m_AnimationClips.find(id);
    if (it2 == m_AnimationClips.end())
        return false;

    curAnim = id;
    m_prevAnim = m_CurAnim;
    m_CurAnim = it2->second.get();
    PrevTickTime = CurTickTime;
    CurTickTime = 0;
    isBlending = true;
    blendDuration = duration;
    blendAlpha = 0.0f;

    return true;
}

bool Animator::RegisterFrameEvent(const std::string& clipName, UINT frame, AnimationEventCallback callback)
{
    auto it = m_NameToID.find(clipName);
    if (it == m_NameToID.end())
        return false;

    return RegisterFrameEvent(it->second, frame, std::move(callback));
}

bool Animator::RegisterFrameEvent(UINT clipID, UINT frame, AnimationEventCallback callback)
{
    if (!callback)
        return false;

    auto clipIt = m_AnimationClips.find(clipID);
    if (clipIt == m_AnimationClips.end())
        return false;

    if (frame > clipIt->second->duration)
        return false;

    m_FrameEvents[clipID][frame].push_back(std::move(callback));
    return true;
}

bool Animator::ClearFrameEvents(const std::string& clipName)
{
    auto it = m_NameToID.find(clipName);
    if (it == m_NameToID.end())
        return false;

    return ClearFrameEvents(it->second);
}

bool Animator::ClearFrameEvents(UINT clipID)
{
    return m_FrameEvents.erase(clipID) > 0;
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
                    m_GlobalBoneNoOffsetTM,
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
                    m_GlobalBoneNoOffsetTM,
                    XMMatrixIdentity()
                );
            }
            UI::EndDisabled();

            UI::DragInt("FramePerSec", &m_CurAnim->TickPerSec, 1, 1, 60);
        }
    }
}
