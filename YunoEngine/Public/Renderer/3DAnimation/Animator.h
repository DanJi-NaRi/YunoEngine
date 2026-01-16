#pragma once
#include "BoneNode.h"

struct AnimationClip
{
    std::string name;

    UINT TickPerSec;
    UINT duration; //애니메이션 한 사이클 도는 틱 타임

    std::vector<std::unique_ptr<BoneAnimation>> channels;//BoneAnimation벡터의 원소는 각각 본 한개에 대응하는 본 애니메이션을 가지고있음
};

class Animator
{
private:
    std::unique_ptr<BoneNode> m_RootBone;

    std::unordered_map<UINT, std::unique_ptr<AnimationClip>> m_AnimationClips;
    std::unordered_map<std::string, UINT> m_NameToID;
    UINT animCount = 0;
    UINT curAnim = 0;

    std::vector<XMFLOAT4X4> m_BoneTMs;
    UINT m_BoneCount;

    unsigned int TickPerSec = 4800; //매직넘버, 수정 가능
    unsigned int CurTickTime = 0;
public:
    Animator();
    virtual ~Animator();

    void SetBoneTree(std::unique_ptr<BoneNode>&& rootNode, UINT boneCount);
    //겹치는 id 이미 있으면 실패 false 반환
    bool AddAnimationClip(const std::string& id, std::unique_ptr<AnimationClip>&& clip);
    void Update(float dTime);

    const std::vector<XMFLOAT4X4>& GetBoneTMs() { return m_BoneTMs; }
};
