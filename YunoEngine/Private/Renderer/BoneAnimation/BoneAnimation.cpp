#include "pch.h"

#include "BoneAnimation.h"

template<typename T>
int FindKeyIndex(const std::vector<T>& keys, UINT tick)
{
    for (size_t i = 0; i < keys.size() - 1; i++)
    {
        if (tick < keys[i + 1].TickTime)
            return i;
    }

    return keys.size() - 2; // 마지막 구간
}

XMMATRIX BoneAnimation::LerpTransKey()
{
    auto& transKey = m_AnimationClips->TransKeys;

    if (transKey.empty())
        return XMMatrixIdentity();

    if (transKey.size() == 1)
        return XMMatrixTranslation(transKey[0].trans.x, transKey[0].trans.y, transKey[0].trans.z);

    int i = FindKeyIndex<TransKey>(transKey, CurTickTime);
    int j = i + 1;

    float t = (CurTickTime - transKey[i].TickTime) /
        (transKey[j].TickTime - transKey[i].TickTime);

    XMVECTOR prevTrans = XMLoadFloat3(&transKey[i].trans);
    XMVECTOR nextTrans = XMLoadFloat3(&transKey[j].trans);

    return XMMatrixTranslationFromVector(XMVectorLerp(prevTrans, nextTrans, t));
}

XMMATRIX BoneAnimation::SLerpQuatKey()
{
    auto& quatKey = m_AnimationClips->QuatKeys;

    if (quatKey.empty())
        return XMMatrixIdentity();

    if (quatKey.size() == 1)
    {
        XMVECTOR quat = XMLoadFloat4(&quatKey[0].quat);
        return XMMatrixRotationQuaternion(quat);
    }
        
    int i = FindKeyIndex<QuatKey>(quatKey, CurTickTime);
    int j = i + 1;

    float t = (CurTickTime - quatKey[i].TickTime) /
        (quatKey[j].TickTime - quatKey[i].TickTime);

    XMVECTOR prevQuat = XMLoadFloat4(&quatKey[i].quat);
    XMVECTOR nextQuat = XMLoadFloat4(&quatKey[j].quat);

    XMVECTOR q = XMQuaternionSlerp(prevQuat, nextQuat, t);
    q = XMQuaternionNormalize(q);

    return XMMatrixRotationQuaternion(q);
}

XMMATRIX BoneAnimation::LerpScaleKey()
{
    auto& scaleKey = m_AnimationClips->ScaleKeys;

    if (scaleKey.empty())
        return XMMatrixIdentity();

    if (scaleKey.size() == 1)
        return XMMatrixScaling(scaleKey[0].scale.x, scaleKey[0].scale.y, scaleKey[0].scale.z);

    int i = FindKeyIndex<ScaleKey>(scaleKey, CurTickTime);
    int j = i + 1;

    float t = (CurTickTime - scaleKey[i].TickTime) /
        (scaleKey[j].TickTime - scaleKey[i].TickTime);

    XMVECTOR prevScale = XMLoadFloat3(&scaleKey[i].scale);
    XMVECTOR nextScale = XMLoadFloat3(&scaleKey[j].scale);

    return XMMatrixScalingFromVector(XMVectorLerp(prevScale, nextScale, t));
}

void BoneAnimation::Init()
{
    if (m_AnimationClips)
        frameCount = std::max(std::max(m_AnimationClips->QuatKeys.size(), m_AnimationClips->TransKeys.size()), m_AnimationClips->ScaleKeys.size());
}

const XMMATRIX& BoneAnimation::Update(float CurrentTick)
{
    CurTickTime = CurrentTick;
    // 본행렬 계산
    {
        XMMATRIX scale = LerpScaleKey();
        XMMATRIX quat = SLerpQuatKey();
        XMMATRIX trans = LerpTransKey();

        m_mAnim = scale * quat * trans;
    }

    return m_mAnim;
}
