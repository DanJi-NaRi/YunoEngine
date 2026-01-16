#pragma once

struct TransKey
{
    XMFLOAT3 trans;
};

//쿼터니언
struct QuatKey
{
    XMFLOAT4 quat;
};

struct ScaleKey
{
    XMFLOAT3 scale = XMFLOAT3(1, 1, 1);
};

struct AniClip
{
    unsigned int TickTime = 0;
    TransKey transKey;
    QuatKey quatKey;
    ScaleKey scaleKey;
};

//파싱 후 저장용
struct AnimationClips
{
    std::string boneName;

    std::vector<AniClip> Clips;
};

class BoneAnimation
{
private:
    std::string boneName;

    unsigned int CurTickTime = 0;

    size_t CurFrame = 0;
    size_t NextFrame = 1;
    size_t LastFrame = 2;

    float frameTime;

    std::vector<AniClip> m_AnimationClips;

    XMFLOAT4X4 m_mAnim; //애니메이션 행렬-> 본의 로컬 공간

    XMMATRIX LerpTransKey();
    XMMATRIX SLerpQuatKey();
    XMMATRIX LerpScaleKey();
public:
    BoneAnimation() = delete;
    virtual ~BoneAnimation() = default;

    BoneAnimation(AnimationClips clips) : boneName(clips.boneName), m_AnimationClips(clips.Clips),
        CurTickTime(0) {
        Init();
    }

    void Init();

    const XMFLOAT4X4& Update(unsigned int CurTickTime);
};
