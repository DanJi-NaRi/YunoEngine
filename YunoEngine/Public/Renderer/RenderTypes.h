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
    ShadowMap,     // 슬롯 4
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

enum YunoTopology : uint8_t
{
    Yuno_LINELIST,
    Yuno_LINESTRIP,
    Yuno_TRIANGLELIST,
    Yuno_TRIANGLESTRIP,
    yuno_Count
};

struct VertexStreams
{
    uint32_t flags = 0;         
    uint32_t vtx_count = 0;                 // 정점 개수
    YunoTopology topology = Yuno_TRIANGLELIST; // 선택 옵션

    const VERTEX_Pos* pos = nullptr;
    const VERTEX_Nrm* nrm = nullptr;
    const VERTEX_UV* uv = nullptr;
    const VERTEX_T* t = nullptr;
    const VERTEX_B* b = nullptr;
    const VERTEX_BoneWeight* boneWeight = nullptr;
    const VERTEX_BoneIndex* boneIdx = nullptr;
};

#define MAX_BONE 16
struct Update_Data
{
    DirectX::XMFLOAT4X4 world{};
    DirectX::XMFLOAT4X4 boneAnim[MAX_BONE] = {};
    DirectX::XMFLOAT4 baseColor = { 1, 1, 1, 1 };
    DirectX::XMFLOAT4 emissiveColor = { 1, 1, 1, 1 };
    XMFLOAT3 worldPos = { 0, 0, 0 }; //반투명 오브젝트 정렬용
    float roughRatio = 1.0f;
    float metalRatio = 1.0f;
    float shadowBias = 0.001f;
    float opacity = 1.0f;
    float emissive = 1.0f;
    /////// 위젯 ///////
    XMFLOAT2 widgetSize = { 0, 0 }; //★
    float widgetValueFloat;  // 자유 사용 빈공간 ★
    int   widgetValueInt;    // 자유 사용 빈공간 ★
    //XMFLOAT2 _widgetPad0 = { 0, 0 }; // 패딩. 다른 용도로도 사용 가능★
    //XMFLOAT4 widgetValueFloat;  // 자유 사용 빈공간 ★
    //XMINT4   widgetValueInt;    // 자유 사용 빈공간 ★
    ///////////////////
};

struct Effect_Data
{
    XMFLOAT4 effectData; // x = frameIndex, y = time, // z = cols, // w = rows
};

struct Frame_Data_Dir
{
    XMFLOAT4 Lightdir;
    XMFLOAT4 Lightdiff;
    XMFLOAT4 Lightamb;
    XMFLOAT4 Lightspec;
    float    intensity;
};

struct Frame_Data_Point
{
    XMFLOAT4 col;
    XMFLOAT3 pos;
    float intensity;
};



// 게임 >> 엔진 렌더 제출 단위
struct RenderItem
{
    MeshHandle meshHandle = 0;
    MaterialHandle materialHandle = 0;

    Update_Data Constant;
    Effect_Data effectConst;
    float sortkey;
    bool isEffect;
    bool isEmissive = false;
    bool haveAnim = false;
    bool castShadow = true; //그림자맵에 그릴 오브젝트
    bool receiveShadow = true; //그림자가 적용될 오브젝트(파티클, 반투명 오브젝트 제외용)
    bool isWidget = false;  // 위젯만 갱신(b6) // ★
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

enum class FontID : uint8_t
{
    Default = 0,


    Count
};

struct TextDrawDesc
{
    FontID fontId = FontID::Default;
    std::wstring text;
    XMFLOAT2 position = { 0.0f, 0.0f };
    XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMFLOAT2 scale = { 1.0f, 1.0f };
    float rotation = 0.0f;
    XMFLOAT2 origin = { 0.0f, 0.0f };
    float layerDepth = 0.0f;
};

enum class ShaderId : uint8_t
{
    Basic = 0,
    PBRBase,
    BasicAnimation,
    PBRAnimation,
    Unlit,
    Skybox,
    UIBase,
    UIGauge,
    UISpriteSheet,
    EffectBase,

    //Shadow
    ShadowPass,
    ShadowPassSkinning,

    //PostProcess
    PP_Default,
    PP_Threshold,
    PP_DownSample ,
    PP_BlurH, 
    PP_BlurV, 
    PP_Combine, 
    PP_ToneMap, 

    //Debug
    DebugGrid,
    DebugMesh,

    None,

    Count
};

enum class MaterialDomain
{
    Surface,
    PostProcess
};

struct PassOption
{
    ShaderId shader = ShaderId::None;
    BlendPreset blend = BlendPreset::Opaque;
    RasterPreset raster = RasterPreset::CullBack;
    DepthPreset depth = DepthPreset::ReadWrite;
};

struct PassKey
{
    ShaderId vs = ShaderId::Basic;
    ShaderId ps = ShaderId::Basic;
    uint32_t vertexFlags = 0;
    BlendPreset blend = BlendPreset::Opaque;
    RasterPreset raster = RasterPreset::CullBack;
    DepthPreset depth = DepthPreset::ReadWrite;
    MaterialDomain domain = MaterialDomain::Surface;

    bool operator==(const PassKey& rhs) const
    {
        return vs == rhs.vs
            && ps == rhs.ps
            && vertexFlags == rhs.vertexFlags
            && blend == rhs.blend
            && raster == rhs.raster
            && depth == rhs.depth
            && domain == rhs.domain;
    }
};



