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
    Hit,
    Target,
    TargetEnemy,
    HitBlue,
    Lightning,
    Razer,
    BlasterAttack,
    BlasterAttackEnemy,
    AxAttack,
    AxAttack2,
    AxAttackEnemy,
    AxAttackEnemy2,
    Buff,
    StaminaBuff,
    PowerUpBuff,
    ShieldBuff,
    DeBuff,
    DrillAttack1,
    DrillAttack2,
    DrillAttackEnemy1,
    DrillAttackEnemy2,
    PeacePosAlly,
    PeacePosEnemy,
    Warning,
    ScytheAttack,
    ScytheAttack2,
    ScytheAttackEnemy,
    ScytheAttackEnemy2,
    ChakramAttack01_1,
    ChakramAttack01_2,
    ChakramAttackEnemy01_1,
    ChakramAttackEnemy01_2,
    ChakramAttack02_1,
    ChakramAttack02_2,
    ChakramAttackEnemy02_1,
    ChakramAttackEnemy02_2,
    FloorWarning1,
    FloorWarning2,

    Count
};

struct EffectDesc
{
    std::wstring texPath = L"";
    EffectID id = EffectID::Default;
    ShaderId shaderid = ShaderId::EffectBase;
    BillboardMode billboard = BillboardMode::ScreenAligned;
    float lifetime = 1.0f;
    float emissive = 0.5f;
    XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMFLOAT3 rot = { 0, 0, 0 };
    int framecount = 0;
    int cols = 0;
    int rows = 0;
    bool isLoop = false;
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
