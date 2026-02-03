#pragma once
#include "BoneAnimation.h"

class BoneNode
{
private:
    std::string m_name;

    BoneNode* m_Parent;
    std::vector<std::unique_ptr<BoneNode>> m_Childs;

    XMMATRIX m_mUser;     //애니메이션만이 누적된 행렬
    XMMATRIX m_BoneOffset; //정점을 본의 로컬로 보내는 행렬을 CPU에서 미리 누적
    XMMATRIX m_BindLocal;

    int boneIndex;
public:
    BoneNode() = delete;
    virtual ~BoneNode() = default;

    BoneNode(const std::string& name, int index) : m_name(name), boneIndex(index), m_BindLocal(XMMatrixIdentity()), m_BoneOffset(XMMatrixIdentity()) {}

    void SetParent(BoneNode* parent) { m_Parent = parent; }
    void Attach(std::unique_ptr<BoneNode>&& child);

    void SetBoneOffset(const XMMATRIX& mOffset) { m_BoneOffset = mOffset; }
    void SetBindLocal(const XMMATRIX& mBindLocal) { m_BindLocal = mBindLocal; }
    const XMMATRIX& GetBindLocal() { return m_BindLocal; }

    void SampleLocalPose(float CurTickTime, std::vector<std::unique_ptr<BoneAnimation>>& clip, std::vector<XMMATRIX>& outArr);

    void UpdateBoneMatrix(const std::vector<XMMATRIX>& localPose, std::vector<XMFLOAT4X4>& outFinal, std::vector<XMFLOAT4X4>& outGlobal, const XMMATRIX& parentGlobal);

    const XMMATRIX& GetBoneTM() { return m_mUser; }
};
