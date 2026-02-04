#pragma once

#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "YunoEngine.h"
#include "IRenderer.h"
#include "YunoCamera.h"
#include "YunoConstantBuffers.h"
#include "YunoMaterial.h"
#include "MaterialDesc.h"




enum ClearColor { Black, White, Gray, Red, Blue, Green, MAX_COUNT };

struct RenderTarget
{
    ComPtr<ID3D11Texture2D> tex;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11ShaderResourceView> srv;
    uint32_t w = 0, h = 0;
    DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
};

enum PostProcessFlag : uint32_t
{
    Default = 1u << 0,
    Bloom = 1u << 1,

    Max
};

struct PostProcessPass
{
    MaterialHandle   material;
    UINT rtIdx;
    bool enabled = true;
};

struct ShadowMap
{
    uint32_t width;
    uint32_t height;

    DXGI_FORMAT texFormat = DXGI_FORMAT_R32_TYPELESS;
    DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
    DXGI_FORMAT srvFormat = DXGI_FORMAT_R32_FLOAT;

    ComPtr<ID3D11Texture2D> dstex;
    ComPtr<ID3D11DepthStencilView> dsv;
    ComPtr<ID3D11ShaderResourceView> dssrv;
};


// 전방선언
class IWindow;
class YunoRenderPass;
class YunoMeshBuffer;
class YunoShader;
struct PostProcessDesc;

namespace
{
    struct ImageRGBA8
    {
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> pixels; // RGBA8, row-major
    };

    // WIC: file -> RGBA8 메모리
    bool LoadImageRGBA8_WIC(const wchar_t* path, ImageRGBA8& out)
    {
        out = {};

        if (!path || !path[0])
            return false;

        // COM 초기화 (이미 되어 있으면 S_FALSE/이미 초기화 상태 OK)
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
            return false;

        Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(factory.GetAddressOf())
        );
        if (FAILED(hr))
            return false;

        Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
        hr = factory->CreateDecoderFromFilename(
            path,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            decoder.GetAddressOf()
        );
        if (FAILED(hr))
            return false;

        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
        hr = decoder->GetFrame(0, frame.GetAddressOf());
        if (FAILED(hr))
            return false;

        UINT w = 0, h = 0;
        hr = frame->GetSize(&w, &h);
        if (FAILED(hr) || w == 0 || h == 0)
            return false;

        // RGBA8로 변환
        Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
        hr = factory->CreateFormatConverter(converter.GetAddressOf());
        if (FAILED(hr))
            return false;

        hr = converter->Initialize(
            frame.Get(),
            GUID_WICPixelFormat32bppRGBA, // 최종 RGBA8
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom
        );
        if (FAILED(hr))
            return false;

        const uint32_t stride = w * 4;
        const uint32_t sizeBytes = stride * h;

        out.width = static_cast<uint32_t>(w);
        out.height = static_cast<uint32_t>(h);
        out.pixels.resize(sizeBytes);

        hr = converter->CopyPixels(
            nullptr,
            stride,
            sizeBytes,
            out.pixels.data()
        );

        return SUCCEEDED(hr);
    }
}//namespace


class YunoRenderer final : public IRenderer
{
public:
    YunoRenderer();
    ~YunoRenderer() override;

    bool Initialize(IWindow* window) override;
    void BeginFrame() override;
    void EndFrame() override;
    void Resize(uint32_t width, uint32_t height) override;
    void Shutdown() override;

    bool SetMSAASamples(uint32_t samples) override;
    uint32_t GetMSAASamples() const override { return m_msaaSamples; }

    // --- Game -> Engine ---
    MeshHandle CreateMesh(const VertexStreams& streams,
        const INDEX* triangles,
        uint32_t triCount) override;

    MaterialHandle CreateMaterial_Default() override;
    MaterialHandle CreateMaterial(const MaterialDesc& desc) override;

    TextureHandle CreateColorTexture2DFromFile(const wchar_t* path) override;
    TextureHandle CreateDataTexture2DFromFile(const wchar_t* path) override;

    void Submit(const RenderItem& item) override;
    void SubmitText(const TextDrawDesc& desc) override;
    void Flush() override;  

#ifdef _DEBUG
    void DrawDebug();
    void RegisterDrawUI(); //imgui드로우 함수 등록

#endif
public:

    void BindConstantBuffers_Camera(const Frame_Data_Dir& dirData) override;
    void BindConstantBuffers_Light(const Frame_Data_Dir& dirData, const std::vector<Frame_Data_Point>& plData, UINT plCount) override;

    YunoCamera& GetCamera() override { return m_camera; }
    std::pair<int, int> GetTextureSize(TextureHandle handle) const;
    
    void SetPostProcessOption(const PostProcessDesc& opt) override; //Use PostProcessFlag (기존 설정된 옵션 있으면 +, 없으면 Set)
    PostProcessDesc GetPostProcessOption() override;
    void ResetPostProcessOption() override; //Reset PostProcessOption
private:
    void BindConstantBuffers(const RenderItem& item);
    void DrawTextBatch();

private:
    bool CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    bool CreateRenderTarget();
    bool CreateDepthStencil(uint32_t width, uint32_t height);
    bool CreateBasicPipeline();
    bool CreateShaders();

    void SetViewPort();
    void SetViewPort(uint32_t width, uint32_t height);
    void ClearDepthStencil();
    const ShaderId SetShaderKey(const MaterialDesc& desc);

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;

private:
    RenderTarget m_backBufferRT;

    Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_backBufferTex;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_depthStencilTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

    YunoConstantBuffer<CBPerFrame> m_cbFrame;
    YunoConstantBuffer<CBPerObject_Matrix> m_cbObject_Matrix;
    YunoConstantBuffer<CBPerObject_Material> m_cbObject_Material;
    YunoConstantBuffer<CBLight_All> m_cbLight;
    YunoConstantBuffer<CBEffect> m_cbEffect;

    CBLight_All m_LightInfo;

    YunoCamera m_camera;
    float m_aspect = 1.0f;

    // MSAA
private:

    uint32_t m_msaaSamples = 8;     // 1/2/4/8 (1이면 MSAA off)
    uint32_t m_msaaQuality = 0;

    // MSAA 렌더 타겟(오프스크린)
    RenderTarget m_msaaRT;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_msaaDepthTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_msaaDSV;

    bool CreateMSAARenderTarget(uint32_t width, uint32_t height);
    bool CreateMSAADepthStencil(uint32_t width, uint32_t height);
    bool CheckMSAA();

    //ShadowMap
private:
    ShadowMap m_ShadowMap;
    YunoConstantBuffer<CBShadow> m_cbShadow;
    CBShadow m_shadowInfo;
    YunoCamera m_shadowCamera;
    float m_shadowBias = 0.0001f;

    PassKey m_ShadowPassKey;
    RenderPassHandle m_ShadowPass;
    PassKey m_ShadowSkinPassKey;
    RenderPassHandle m_ShadowSkinPass;

    bool CreateShadowMap(uint32_t width, uint32_t height);
    void InitShadowPass();

    void DrawShadowMap();

    //PostProcessing
private:
    RenderTarget m_sceneRT;
    RenderTarget m_postRT[2]; //chain방식
    RenderTarget m_bloomRT[4]; // 각각 해상도 다른 렌더타겟
    RenderTarget m_blurTemp[4]; // 각 bloomRT에 대응

    uint32_t m_postIndex = 0;

    RenderTarget& CurPostRT();
    RenderTarget& NextPostRT();

    //PP Init
    bool CreatePPRenderTarget(uint32_t width, uint32_t height);
    bool CreateDefaultPPRT(uint32_t width, uint32_t height);
    bool CreateBloomRT(uint32_t width, uint32_t height);
    bool CreatePPShader();
    bool CreatePPMaterial();
    bool CreatePPCB();
    void SetPP_Pass();

    //PP Process
    void PostProcess();
    ID3D11ShaderResourceView* PostProcessScene();
    ID3D11ShaderResourceView* PostProcessBloom(ID3D11ShaderResourceView* input);
    void PostProcessFinal(ID3D11ShaderResourceView* input);
    void UnBindAllSRV();
    void BindRT(ID3D11RenderTargetView* rt);

    //Bloom
    void BindBloomThreshold(ID3D11ShaderResourceView* input);
    void BindBloomDownSample(ID3D11ShaderResourceView* input);
    void BindBloomBlurH(ID3D11ShaderResourceView* input);
    void BindBloomBlurV(ID3D11ShaderResourceView* input);
    void BindBloomCombine(ID3D11ShaderResourceView* input);

    MaterialHandle m_ppThresholdMat = 0;
    MaterialHandle m_ppDownSampleMat = 0;
    MaterialHandle m_ppBlurHMat = 0; //Horizontal
    MaterialHandle m_ppBlurVMat = 0; //Vertical
    MaterialHandle m_ppCombineMat = 0;
    MaterialHandle m_ppToneMapMat = 0;

    float m_Threshold = 1.05f; //추출할 최소 밝기값
    float m_BloomIntensity = 0.1f; //Bloom 빛 번짐정도
    float m_Exposure = 1.3f; //전체 화면 밝기조절 값
    float m_blurRadius[4] = { 2.0f, 1.0f, 0.5f, 0.2f };

    //PP Default
    uint32_t m_PPFlag;
    std::unordered_map<PostProcessFlag, MaterialHandle> m_PPMaterials;
    std::vector<PostProcessPass> m_ppChain;
    MaterialHandle m_ppDefaultMat = 0;
    YunoConstantBuffer<CBPostProcess> m_ppCB;
    YunoConstantBuffer<CBBloom> m_ppCBloom;

public:
    void SetThreshold(float v) { m_Threshold = v; }
    void SetExposure(float v) { m_Exposure = v; }
    
    // 나중에 메쉬 매니저 이런걸로 뺼듯
private:
    struct MeshResource
    {
        uint32_t flags = 0;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        // 스트림별 Vertex Buffer
        ComPtr<ID3D11Buffer> vbPos;
        ComPtr<ID3D11Buffer> vbNrm;
        ComPtr<ID3D11Buffer> vbUV;
        ComPtr<ID3D11Buffer> vbT;
        ComPtr<ID3D11Buffer> vbB;
        ComPtr<ID3D11Buffer> vbBoneWeight;
        ComPtr<ID3D11Buffer> vbBoneIndex;

        // Index Buffer (optional)
        ComPtr<ID3D11Buffer> ib;

        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        void Bind(ID3D11DeviceContext* ctx) const
        {
            // IA 슬롯 고정 매핑:
            // 0=Pos, 1=Nrm, 2=UV, 3=T, 4=B
            ID3D11Buffer* vbs[7] = {};
            UINT strides[7] = {};
            UINT offsets[7] = {};

            // Pos는 필수라고 가정 (CreateMesh에서 보장)
            vbs[0] = vbPos.Get(); strides[0] = sizeof(VERTEX_Pos);

            if (vbNrm) { vbs[1] = vbNrm.Get(); strides[1] = sizeof(VERTEX_Nrm); }
            if (vbUV) { vbs[2] = vbUV.Get();  strides[2] = sizeof(VERTEX_UV); }
            if (vbT) { vbs[3] = vbT.Get();   strides[3] = sizeof(VERTEX_T); }
            if (vbB) { vbs[4] = vbB.Get();   strides[4] = sizeof(VERTEX_B); }
            if (vbBoneWeight) { vbs[5] = vbBoneWeight.Get();   strides[5] = sizeof(VERTEX_BoneWeight); }
            if (vbBoneIndex) { vbs[6] = vbBoneIndex.Get();   strides[6] = sizeof(VERTEX_BoneIndex); }

            UINT maxSlot = 0;
            if (vbBoneIndex)        maxSlot = 6;
            else if (vbBoneWeight)   maxSlot = 5;
            else if (vbB)   maxSlot = 4;
            else if (vbT)   maxSlot = 3;
            else if (vbUV)  maxSlot = 2;
            else if (vbNrm) maxSlot = 1;
            else            maxSlot = 0;

            ctx->IASetVertexBuffers(0, maxSlot + 1, vbs, strides, offsets);

            if (ib && indexCount > 0)
                ctx->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);

            ctx->IASetPrimitiveTopology(topology);
        }
    };

    bool CreateDefaultQuadMesh();
    MeshHandle GetQuadMesh() override;

    std::vector<MeshResource> m_meshes;        // handle -> m_meshes[handle-1]
    std::vector<RenderItem>   m_renderQueue;   // 이번 프레임 제출된 드로우 요청
    std::vector<RenderItem>   m_renderBlendQueue;   // 반투명 메쉬 별도 관리 큐
    std::vector<RenderItem>   m_widgetRenderQueue;

    struct MaterialResource
    {
        //YunoRenderPass* pass = nullptr;
        RenderPassHandle pass = 0; // handle -> m_passes[pass-1]
    };

    std::vector<YunoMaterial> m_materials;
    MaterialHandle m_defaultMaterial = 0;
    RenderPassHandle m_defaultPass = 0;
    MeshHandle m_defaultQuadMesh = 0;

public:
    YunoMaterial& GetMaterial(MaterialHandle handle) { 
        assert(handle <= m_materials.size() && handle != 0);
        return m_materials[handle - 1]; 
    }

    // 렌더패스 관련 프리셋들
private:

    struct PassKeyHash
    {
        size_t operator()(const PassKey& k) const noexcept
        {
            size_t h = 1469598103934665603ull; // FNV offset basis (64-bit)
            auto Mix = [&](uint64_t v)
                {
                    h ^= v;
                    h *= 1099511628211ull;
                };

            Mix(static_cast<uint64_t>(k.vs));
            Mix(static_cast<uint64_t>(k.ps));
            Mix(static_cast<uint64_t>(k.vertexFlags));
            Mix(static_cast<uint64_t>(k.blend));
            Mix(static_cast<uint64_t>(k.raster));
            Mix(static_cast<uint64_t>(k.depth));
            return h;
        }
    };

    using RenderPassHandle = uint32_t;
    
public:
    // 캐시 조회/생성
    RenderPassHandle GetOrCreatePass(const PassKey& key);
private:

    std::vector<std::unique_ptr<YunoRenderPass>> m_passes;
    std::unordered_map<PassKey, RenderPassHandle, PassKeyHash> m_passCache;

    // Preset states cache (필요할 때 생성)
    Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendStates[(size_t)BlendPreset::Count]{};
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_rasterStates[(size_t)RasterPreset::Count]{};
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStates[(size_t)DepthPreset::Count]{};

    Microsoft::WRL::ComPtr<ID3DBlob> m_basicVSBytecode;

    bool InputLayoutFromFlags(uint32_t flags,
        std::vector<D3D11_INPUT_ELEMENT_DESC>& outLayout) const;

private:
    struct ShaderProgram
    {
        std::unique_ptr<YunoShader> vs;
        std::unique_ptr<YunoShader> ps;
        Microsoft::WRL::ComPtr<ID3DBlob> vsBytecode; // InputLayout용
    };

    std::unordered_map<ShaderId, ShaderProgram> m_programs;

public:
    bool LoadShader(
        ShaderId id,
        const std::wstring& filePath,   // ex) L"Assets/Shaders/BasicColor.hlsl"
        const std::string& vsEntry,
        const std::string& psEntry,
        const std::string& vsProfile = "vs_5_0",
        const std::string& psProfile = "ps_5_0",
        const std::vector<std::pair<std::string, std::string>>& defines = {});
    bool LoadShader(
        ShaderId id,
        const char* filePath,     // "../Assets/Shaders/BasicColor.hlsl"
        const char* vsEntry,      // "VSMain"
        const char* psEntry,      // "PSMain"
        const char* vsProfile = "vs_5_0",
        const char* psProfile = "ps_5_0");

// 텍스쳐
private:
    struct TextureResource
    {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        uint32_t w = 0;
        uint32_t h = 0;

        uint32_t GetWidth() const { return w; }
        uint32_t GetHeight() const { return h; }
    };

    std::vector<TextureResource> m_textures; // handle -> m_textures[handle-1]

    // 더미 텍스처 핸들(0은 "없음"이므로 실제 더미는 1-based handle로 보관)
    TextureHandle m_texWhite = 0; // 알베도 기본
    TextureHandle m_texNormal = 0; // 노멀 기본(0,0,1)
    TextureHandle m_texBlack = 0; // orm 기본(0,0,0) 또는 (1,1,1) 필요 시 변경

    bool CreateSamplerPresets();

    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplers[(uint8_t)SampleMode::Count];
    std::unordered_map<std::wstring, TextureHandle> m_texturePathCache;
private:
    TextureHandle CreateTexture2D_SolidRGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    bool CreateDefaultTextures();    // white/normal/black 생성


    ID3D11ShaderResourceView* ResolveSRV(TextureHandle h) const;

    void BindTextures(const YunoMaterial& material);
    void BindSamplers();

    //Font
private:
    bool RegisterFont(); //모든 폰트 등록은 여기서

    std::unordered_map<FontID, std::unique_ptr<SpriteFont>> m_Fonts;
    std::unique_ptr<SpriteBatch> m_SpriteBatch;
    std::vector<TextDrawDesc> m_textQueue;

private:
    // Debug Grid (Engine-owned)
#ifdef _DEBUG
    bool m_enableDebugGrid = true;

    MeshHandle          m_debugGridMeshHandle = 0;
    MaterialHandle      m_debugGridMaterial = 0;
    RenderPassHandle    m_debugGridPass = 0;

    //DebugMesh
    MeshHandle      m_debugSphereMeshHandle = 0;
    MaterialHandle m_debugMeshMaterial = 0;

    //DebugPointLight
    struct DebugLight {
        XMFLOAT3 pos;
        XMFLOAT4 col;
    };

    std::vector<DebugLight> debuglights;
#endif


private:
#ifdef _DEBUG
    void CreateDebugGridResources();
    void CreateDebugMeshResources();
    void SubmitDebugGrid();
    void SubmitDebugPointLightMesh();
#endif
    MeshHandle CreateLineMesh_PosOnly(const VERTEX_Pos* verts, uint32_t vtxCount);

    friend YunoEngine;
};
