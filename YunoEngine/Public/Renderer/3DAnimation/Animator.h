#pragma once
#include "BoneNode.h"


struct AnimationClip
{
    std::string name;

    UINT TickPerSec;
    UINT duration; //애니메이션 한 사이클 도는 틱 타임
    bool isLoop;

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

    AnimationClip* m_CurAnim;

    std::vector<XMFLOAT4X4> m_BoneTMs;
    UINT m_BoneCount;

    float CurTickTime = 0;

    bool isPlay = true;
public:
    Animator();
    virtual ~Animator();

    void Play() { isPlay = true; }
    void Stop() { isPlay = false; }

    void SetBoneTree(std::unique_ptr<BoneNode>&& rootNode, UINT boneCount);
    //겹치는 id 이미 있으면 실패 false 반환
    bool AddAnimationClip(const std::string& id, std::unique_ptr<AnimationClip>&& clip);
    bool AddAnimationFromFile(const std::string& name, const std::wstring& filepath);
    void Update(float dTime);

    UINT GetAnimationNum() { return m_AnimationClips.size(); }

    void ChangeAnimation(UINT id);

    UINT GetCurAnimFrameCount();

    void Serialize();

    const std::vector<XMFLOAT4X4>& GetBoneTMs() { return m_BoneTMs; }
};
