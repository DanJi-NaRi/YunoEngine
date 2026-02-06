#pragma once
#include "RenderTypes.h"

enum class BillboardMode
{
    None,                   // 빔, 링 등
    ScreenAligned,   // 폭발 (카메라 정면)
    WorldAligned,    // 불꽃 기둥 (Y축 고정)
    AxisLocked,          // 특정 축만 회전
    Beam                    // 가로 레이저 하드코딩
};

enum class EffectID : uint32_t
{
    Default,
    HitRed,
    HitBlue,
    Lightning,
    Razer,
    BlasterAttack,
    AxAttack,
    Buff,
    DrillAttack1,
    DrillAttack2,
    PeacePosAlly,
    PeacePosEnemy,

    Count
};

struct EffectTemplate
{
    BillboardMode billboard;

    MeshHandle mesh;
    MaterialHandle material;
    TextureHandle texture;

    float lifetime;
    float emissive;
    XMFLOAT4 color;
    XMFLOAT3 rot;
    int frameCount;
    int cols;
    int rows;
    bool isLoop;
};
