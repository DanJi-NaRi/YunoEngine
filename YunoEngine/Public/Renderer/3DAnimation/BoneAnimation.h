#pragma once

struct TransKey
{
    float TickTime = 0;
    XMFLOAT3 trans;
};

//쿼터니언
struct QuatKey
{
    float TickTime = 0;
    XMFLOAT4 quat;
};

struct ScaleKey
{
    float TickTime = 0;
    XMFLOAT3 scale = XMFLOAT3(1, 1, 1);
};

//파싱 후 저장용
struct AnimationClips
{
    std::string boneName;

    std::vector<TransKey> TransKeys;
    std::vector<QuatKey> QuatKeys;
    std::vector<ScaleKey> ScaleKeys;
};

class BoneAnimation
{
private:
    std::string boneName;

    float CurTickTime = 0;

    std::unique_ptr<AnimationClips> m_AnimationClips;

    XMFLOAT4X4 m_mAnim; //애니메이션 행렬-> 본의 로컬 공간

    XMMATRIX LerpTransKey();
    XMMATRIX SLerpQuatKey();
    XMMATRIX LerpScaleKey();
public:
    BoneAnimation() = delete;
    virtual ~BoneAnimation() = default;

    BoneAnimation(std::unique_ptr<AnimationClips>&& clips) : boneName(clips->boneName), m_AnimationClips(std::move(clips)),
        CurTickTime(0) {
        Init();
    }

    UINT frameCount;

    void Init();

    const XMFLOAT4X4& Update(float CurrentTick);
};
