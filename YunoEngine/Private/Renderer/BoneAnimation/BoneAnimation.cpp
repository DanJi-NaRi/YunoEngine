#include "pch.h"

#include "BoneAnimation.h"

XMMATRIX BoneAnimation::LerpTransKey()
{
    XMVECTOR prevTrans = XMLoadFloat3(&m_AnimationClips[CurFrame].transKey.trans);
    XMVECTOR nextTrans = XMLoadFloat3(&m_AnimationClips[NextFrame].transKey.trans);

    return XMMatrixTranslationFromVector(XMVectorLerp(prevTrans, nextTrans, frameTime));
}

XMMATRIX BoneAnimation::SLerpQuatKey()
{
    XMVECTOR prevQuat = XMLoadFloat4(&m_AnimationClips[CurFrame].quatKey.quat);
    XMVECTOR nextQuat = XMLoadFloat4(&m_AnimationClips[NextFrame].quatKey.quat);

    return XMMatrixRotationQuaternion(XMQuaternionSlerp(prevQuat, nextQuat, frameTime));
}

XMMATRIX BoneAnimation::LerpScaleKey()
{
    XMVECTOR prevScale = XMLoadFloat3(&m_AnimationClips[CurFrame].scaleKey.scale);
    XMVECTOR nextScale = XMLoadFloat3(&m_AnimationClips[NextFrame].scaleKey.scale);

    return XMMatrixScalingFromVector(XMVectorLerp(prevScale, nextScale, frameTime));
}

void BoneAnimation::Init()
{
    CurFrame = 0;
    NextFrame = 1;

    LastFrame = m_AnimationClips.size() - 1;
}

const XMFLOAT4X4& BoneAnimation::Update(unsigned int CurTickTime)
{
    //dTime -> TickTime으로 변환

    //TickTime이 현재 키프레임, 다음 키프레임 사이인지 비교
    //사이값이면 보간
    //CurTickTime >= 다음 키프레임이면 프레임 이동
    if (CurTickTime >= m_AnimationClips[NextFrame].TickTime)
    {
        CurFrame++;
        NextFrame++;

        //루프 애니랑 아닌거랑 분기 나눠야할듯
        //이거 여기 냅두는지 아니면 애니메이터로 뺄지 고민
        //나중에 블랜딩하려면 총 프레임이랑 그런거 빼는게 편할것같음
        if (CurFrame == LastFrame)
        {
            CurFrame = 0;
            NextFrame = 1;
        }
    }

    UINT frameStart = m_AnimationClips[CurFrame].TickTime;
    UINT frameEnd = m_AnimationClips[NextFrame].TickTime;

    frameTime = (float)(CurTickTime - frameStart) / (frameEnd - frameStart); //0 ~ 1;

    // 본행렬 계산
    {
        XMMATRIX scale = LerpScaleKey();
        XMMATRIX quat = SLerpQuatKey();
        XMMATRIX trans = LerpTransKey();

        XMMATRIX AnimTM = scale * quat * trans;

        XMStoreFloat4x4(&m_mAnim, AnimTM);
    }

    return m_mAnim;
}
