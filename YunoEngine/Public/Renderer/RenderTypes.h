#pragma once
#include <cstdint>
#include <DirectXMath.h>

// 게임이 들고 있을 핸들
using MeshHandle = uint32_t;
using MaterialHandle = uint32_t;
using RenderPassHandle = uint32_t;

enum class RenderPassBlendMode : uint8_t
{
    Opaque,
    AlphaBlend,
    Additive,
};

enum class RenderPassDepthMode : uint8_t
{
    Disabled,
    ReadOnly,
    ReadWrite,
};

enum class RenderPassCullMode : uint8_t
{
    None,
    Back,
    Front,
};

enum class RenderPassFillMode : uint8_t
{
    Solid,
    Wireframe,
};

// 게임 -> 엔진 렌더패스 생성용 데이터
struct RenderPassDesc
{
    const char* debugName = nullptr;
    uint32_t vertexStreamFlags = 0; // VertexStreamFlags 조합

    RenderPassBlendMode blend = RenderPassBlendMode::Opaque;
    RenderPassDepthMode depth = RenderPassDepthMode::ReadWrite;
    RenderPassCullMode cull = RenderPassCullMode::Back;
    RenderPassFillMode fill = RenderPassFillMode::Solid;

    bool depthClip = true;
    bool alphaToCoverage = false;
};

struct VERTEX_Pos { DirectX::XMFLOAT3 pos; };
struct VERTEX_Nrm { DirectX::XMFLOAT3 nrm; };
struct VERTEX_UV { DirectX::XMFLOAT2 uv; };
struct VERTEX_T { DirectX::XMFLOAT3 T; };
struct VERTEX_B { DirectX::XMFLOAT3 B; };

enum VertexStreamFlags : uint32_t
{
    VSF_Pos = 1u << 0,
    VSF_Nrm = 1u << 1,
    VSF_UV = 1u << 2,
    VSF_T = 1u << 3,
    VSF_B = 1u << 4,
};
// 스트림 사용법 
// flags = VSF_Pos  >> 정점 버퍼에 pos만있음
// flags = VSF_Pos | VSF_Nrm | VSF_UV >> 정점 버퍼에 pos, nrm, uv 있음



// 데이터 전달 방법
/*
VertexStreams s{};
s.flags = VSF_Pos | VSF_Nrm | VSF_UV;
s.count = vertexCount;

s.pos = pos.data();
s.nrm = nrm.data();
s.uv  = uv.data();
*/
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
    MeshHandle mesh = 0;
    MaterialHandle material = 0;
    RenderPassHandle pass = 0;

    DirectX::XMFLOAT4X4 world;
};

// 사용 예시 (게임 측)
/*
RenderPassDesc passDesc{};
passDesc.debugName = "OpaqueLit";
passDesc.vertexStreamFlags = VSF_Pos | VSF_Nrm | VSF_UV;
passDesc.depth = RenderPassDepthMode::ReadWrite;
passDesc.blend = RenderPassBlendMode::Opaque;

RenderPassHandle pass = renderer->CreateRenderPass(passDesc);

RenderItem item{};
item.mesh = mesh;
item.material = material;
item.pass = pass;
renderer->Submit(item);
*/
