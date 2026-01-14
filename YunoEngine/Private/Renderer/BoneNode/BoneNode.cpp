#include "pch.h"

#include "BoneNode.h"

void BoneNode::Attach(std::unique_ptr<BoneNode>& child)
{
    m_Childs.push_back(std::move(child));
    m_Childs[m_Childs.size() - 1]->SetParent(this);
}

void BoneNode::UpdateBoneMatrix(float dTime, std::unordered_map<UINT, std::unique_ptr<BoneAnimation>>& clip, std::vector<XMFLOAT4X4>& outArr, XMMATRIX globalTM)
{
    XMMATRIX animTM = m_BindLocal;

    auto it = clip.find(boneIndex);
    if (it != clip.end() && it->second)
    {
        animTM = XMLoadFloat4x4(&it->second->Update(dTime));
    }

    XMMATRIX userTM = animTM * globalTM;

    for (auto& child : m_Childs)
    {
        child->UpdateBoneMatrix(dTime, clip, outArr, userTM);
    }

    XMMATRIX finalTM = m_BoneOffset * userTM;

    XMStoreFloat4x4(&m_mUser, finalTM);
    
    outArr[boneIndex] = m_mUser;
}
