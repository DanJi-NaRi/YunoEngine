#pragma once
#include "BoneNode.h"

class Animator
{
private:
    std::unique_ptr<BoneNode> m_RootBone;

    std::unordered_map<UINT, std::unordered_map<UINT, std::unique_ptr<BoneAnimation>>> m_AnimationClips;
    UINT curAnim = 0;

    std::vector<XMFLOAT4X4> m_BoneTMs;
    UINT m_BoneCount;
public:
    Animator();
    virtual ~Animator();

    void SetBoneTree(std::unique_ptr<BoneNode> rootNode, UINT boneCount);
    //겹치는 id 이미 있으면 실패 false 반환
    bool AddAnimationClip(unsigned int id, const std::unordered_map<UINT, std::unique_ptr<BoneAnimation>>& clip);
    void Update(float dTime);

    const std::vector<XMFLOAT4X4>& GetBoneTMs() { return m_BoneTMs; }
};
