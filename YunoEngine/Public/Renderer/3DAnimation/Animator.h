#pragma once
#include "BoneNode.h"


struct AnimationClip
{
    std::string name;

    int TickPerSec;
    UINT duration; //애니메이션 한 사이클 도는 틱 타임
    bool isLoop;

    std::vector<std::unique_ptr<BoneAnimation>> channels;//BoneAnimation벡터의 원소는 각각 본 한개에 대응하는 본 애니메이션을 가지고있음
};

class Animator
{
private:
    std::unique_ptr<BoneNode> m_RootBone;

    std::unordered_map<std::string, UINT> m_BoneNameToIndex;
    std::unordered_map<UINT, std::unique_ptr<AnimationClip>> m_AnimationClips;
    std::unordered_map<std::string, UINT> m_NameToID;
    UINT animCount = 0;
    UINT curAnim = 0;

    AnimationClip* m_CurAnim;

    //std::vector<XMFLOAT4X4> m_prevBoneTMs;
    //std::vector<XMFLOAT4X4> m_BoneTMs;
    std::vector<XMMATRIX> m_LocalBoneA;
    std::vector<XMMATRIX> m_LocalBoneB;
    std::vector<XMMATRIX> m_BlendBoneTM;
    std::vector<XMFLOAT4X4> m_FinalBoneTM;
    std::vector<XMFLOAT4X4> m_GlobalBoneTM;
    std::vector<XMFLOAT4X4> m_GlobalBoneNoOffsetTM;
    XMFLOAT4X4 m_Identity;

    UINT m_BoneCount;

    float CurTickTime = 0;

    bool isPlay = true;

    //애니메이션 블랜딩 관련
    bool isBlending = false;
    float blendAlpha = 0.0f;
    float blendDuration = 0.001f;
    float PrevTickTime = 0;
    AnimationClip* m_prevAnim;
public:
    Animator();
    virtual ~Animator();

    void Play() { isPlay = true; }
    void Stop() { isPlay = false; }

    bool isPlaying() { return isPlay; }

    void SetBoneTree(std::unique_ptr<BoneNode>&& rootNode, const std::unordered_map<std::string, UINT>& nameToIndex, UINT boneCount);
    //겹치는 id 이미 있으면 실패 false 반환
    bool AddAnimationClip(const std::string& id, std::unique_ptr<AnimationClip>&& clip);
    bool AddAnimationFromFile(const std::string& name, const std::wstring& filepath);
    void Update(float dTime);
    void BlendLocalPose(const std::vector<XMMATRIX>& A, const std::vector<XMMATRIX>& B, float alpha, std::vector<XMMATRIX>& out);

    bool SetLoop(const std::string& name, bool isLoop);

    bool SetLoop(UINT id, bool isLoop);

    UINT GetAnimationNum() { return m_AnimationClips.size(); }
    UINT GetBoneCount() { return m_BoneCount; }
    const UINT& GetCurFrame() { return static_cast<UINT>(CurTickTime); }
    const XMFLOAT4X4& GetBoneGlobal(int idx);
    const XMFLOAT4X4& GetBoneGlobalNoOffset(int idx);
    int FindIndex(const std::string& name);

    void BlendingUpdate(float dTime);

    bool Change(UINT id, float duration = 0.5f);
    bool Change(const std::string& name, float duration = 0.5f);

    void Serialize();

    const std::vector<XMFLOAT4X4>& GetBoneTMs() { return m_FinalBoneTM; }
};
