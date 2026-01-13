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
};

// 게임 >> 엔진 렌더 제출 단위
struct RenderItem
{
    MeshHandle meshHandle = 0;
    MaterialHandle materialHandle = 0;

    DirectX::XMFLOAT4X4 mWorld;
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
    Unlit,
    PBR,
    Skybox,
    DebugGrid,
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

class Mesh;

struct MeshNode {
    std::string m_name;
    MeshNode* m_Parent = nullptr;
    std::vector<std::unique_ptr<MeshNode>> m_Childs;

    XMFLOAT3 pos;
    XMFLOAT3 rot;
    XMFLOAT3 scale;

    std::vector<std::unique_ptr<Mesh>> m_Meshs;
};
