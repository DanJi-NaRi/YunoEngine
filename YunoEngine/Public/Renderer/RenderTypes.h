#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include <IMesh.h>

// 게임이 들고 있을 핸들
using MeshHandle = uint32_t;
using MaterialHandle = uint32_t;
using TextureHandle = uint32_t;
using RenderPassHandle = uint32_t;



enum class SampleMode : uint8_t
{
    Custom = 0,          // 슬롯 0
    WrapAniso,           // 슬롯 1
    ClampAniso,          // 슬롯 2
    Border0000Aniso,     // 슬롯 3
    Count
};


enum VertexStreamFlags : uint32_t
{
    VSF_Pos = 1u << 0,
    VSF_Nrm = 1u << 1,
    VSF_UV = 1u << 2,
    VSF_T = 1u << 3,
    VSF_B = 1u << 4,
    VSF_BoneIndex = 1u << 5,
    VSF_BoneWeight = 1u << 6
};

struct VertexStreams
{
    uint32_t flags = 0;         
    uint32_t vtx_count = 0;                 // 정점 개수

    const VERTEX_Pos* pos = nullptr;
    const VERTEX_Nrm* nrm = nullptr;
    const VERTEX_UV* uv = nullptr;
    const VERTEX_T* t = nullptr;
    const VERTEX_B* b = nullptr;
    const VERTEX_BoneWeight* boneWeight = nullptr;
    const VERTEX_BoneIndex* boneIdx = nullptr;
};

#define MAX_BONE 70
struct Update_Data
{
    DirectX::XMFLOAT4X4 world{};
    DirectX::XMFLOAT4X4 boneAnim[MAX_BONE] = {};
    DirectX::XMFLOAT4 baseColor = { 1, 1, 1, 1 };
    float roughRatio = 1.0f;
    float metalRatio = 1.0f;
    float shadowBias = 0.005f;
    float padding = 0.0f;
};

struct Frame_Data_Dir
{
    XMFLOAT4 Lightdir;
    XMFLOAT4 Lightdiff;
    XMFLOAT4 Lightamb;
    XMFLOAT4 Lightspec;
    float    intensity;
};



// 게임 >> 엔진 렌더 제출 단위
struct RenderItem
{
    MeshHandle meshHandle = 0;
    MaterialHandle materialHandle = 0;

    Update_Data Constant;
    bool haveAnim = false;
};

// -------------------- PassKey --------------------
enum class BlendPreset : uint8_t
{
    Opaque = 0,
    AlphaBlend,
    ColorBlend,
    ColorBlendOne,
    Count
};

enum class RasterPreset : uint8_t
{
    CullNone = 0,
    CullBack,
    CullFront,
    WireCullNone,
    WireCullBack,
    WireCullFront,
    Count
};

enum class DepthPreset : uint8_t
{
    ReadWrite = 0,
    ReadOnly,
    Off,
    Count
};


enum class ShaderId : uint8_t
{
    Basic = 0,
    PBRBase,
    BasicAnimation,
    PBRAnimation,
    DebugGrid,
    Unlit,
    Skybox,

    Count
};

struct PassKey
{
    ShaderId vs = ShaderId::Basic;
    ShaderId ps = ShaderId::Basic;
    uint32_t vertexFlags = 0;
    BlendPreset blend = BlendPreset::Opaque;
    RasterPreset raster = RasterPreset::CullBack;
    DepthPreset depth = DepthPreset::ReadWrite;

    bool operator==(const PassKey& rhs) const
    {
        return vs == rhs.vs
            && ps == rhs.ps
            && vertexFlags == rhs.vertexFlags
            && blend == rhs.blend
            && raster == rhs.raster
            && depth == rhs.depth;
    }
};



