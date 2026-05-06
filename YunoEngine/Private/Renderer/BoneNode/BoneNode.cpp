#include "pch.h"

#include "BoneNode.h"

void BoneNode::Attach(std::unique_ptr<BoneNode>&& child)
{
    m_Childs.push_back(std::move(child));
    m_Childs[m_Childs.size() - 1]->SetParent(this);
}

void BoneNode::SampleLocalPose(float CurTickTime, std::vector<std::unique_ptr<BoneAnimation>>& clip, std::vector<XMMATRIX>& outArr)
{
    if (boneIndex >= 0 && boneIndex < clip.size())
    {
        if (clip[boneIndex])
            outArr[boneIndex] = clip[boneIndex]->Update(CurTickTime);
        else
            outArr[boneIndex] = m_BindLocal;
    }

    for (auto& child : m_Childs)
    {
        child->SampleLocalPose(CurTickTime, clip, outArr);
    }
}

void BoneNode::UpdateBoneMatrix(
    const std::vector<XMMATRIX>& localPose,
    std::vector<XMFLOAT4X4>& outFinal,
    std::vector<XMFLOAT4X4>& outGlobal,
    std::vector<XMFLOAT4X4>& outGlobalNoOffset,
    const XMMATRIX& parentGlobal)
{
    XMMATRIX local =
        (boneIndex >= 0) ? localPose[boneIndex] : m_BindLocal;

    XMMATRIX global = local * parentGlobal;

    if (boneIndex >= 0 && boneIndex < outFinal.size())
    {
        XMStoreFloat4x4(&outGlobal[boneIndex], m_BoneOffset * global);
        XMVECTOR scale;
        XMVECTOR rotation;
        XMVECTOR translation;
        XMMatrixDecompose(&scale, &rotation, &translation, m_BoneOffset);
        XMMATRIX scaleFix = XMMatrixScalingFromVector(scale);
        //XMMATRIX noOffsetGlobal = XMMatrixScaling(0.01f, 0.01f, 0.01f) * global;
        XMStoreFloat4x4(&outGlobalNoOffset[boneIndex], scaleFix * global);
    }
        

    for (auto& c : m_Childs)
        c->UpdateBoneMatrix(localPose, outFinal, outGlobal, outGlobalNoOffset, global);

    if(boneIndex >= 0 && boneIndex < outFinal.size())
        XMStoreFloat4x4(&outFinal[boneIndex], m_BoneOffset * global);
}
