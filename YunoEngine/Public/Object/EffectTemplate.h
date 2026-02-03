#pragma once
#include "RenderTypes.h"

enum class BillboardMode
{
    None,                   // 빔, 링 등
    ScreenAligned,   // 폭발 (카메라 정면)
    WorldAligned,    // 불꽃 기둥 (Y축 고정)
    AxisLocked          // 특정 축만 회전
};

enum class EffectID : int
{
    Default,
    HitRed,
    HitBlue,
    Lightning,
    Lazer,

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
    int frameCount;
    int cols;
    int rows;
};
