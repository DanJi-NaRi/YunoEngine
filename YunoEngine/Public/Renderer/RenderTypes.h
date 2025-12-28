#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include <IMesh.h>

// 게임이 들고 있을 핸들
using MeshHandle = uint32_t;
using MaterialHandle = uint32_t;
using RenderPassHandle = uint32_t;



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

    DirectX::XMFLOAT4X4 world;
};