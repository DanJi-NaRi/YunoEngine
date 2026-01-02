#pragma once

#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

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


// 전방선언
class IWindow;
class YunoRenderPass;
class YunoMeshBuffer;
class YunoShader;


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
}


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
    MaterialHandle CreateMaterial(const MaterialDesc& desc);

    TextureHandle CreateTexture2DFromFile(const wchar_t* path) override;

    void Submit(const RenderItem& item) override;
    void Flush() override;     

private:
    void BindConstantBuffers(
        const RenderItem& item,
        const YunoMaterial& material
    );


private:
    bool CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    bool CreateRenderTarget();
    bool CreateDepthStencil(uint32_t width, uint32_t height);
    bool CreateBasicPipeline();
    bool CreateShaders();

    void SetViewPort();
    void ClearDepthStencil();



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

    YunoConstantBuffer<CBDefault> m_cbDefault;
    YunoConstantBuffer<CBMaterial> m_cbMaterial;
    YunoCamera m_camera;
    float m_aspect = 1.0f;

    // MSAA
private:

    uint32_t m_msaaSamples = 4;     // 1/2/4/8 (1이면 MSAA off)
    uint32_t m_msaaQuality = 0;

    // MSAA 렌더 타겟(오프스크린)
    RenderTarget m_msaaRT;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_msaaDepthTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_msaaDSV;

    bool CreateMSAARenderTarget(uint32_t width, uint32_t height);
    bool CreateMSAADepthStencil(uint32_t width, uint32_t height);
    bool CheckMSAA();

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

        // Index Buffer (optional)
        ComPtr<ID3D11Buffer> ib;

        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        void Bind(ID3D11DeviceContext* ctx) const
        {
            // IA 슬롯 고정 매핑:
            // 0=Pos, 1=Nrm, 2=UV, 3=T, 4=B
            ID3D11Buffer* vbs[5] = {};
            UINT strides[5] = {};
            UINT offsets[5] = {};

            // Pos는 필수라고 가정 (CreateMesh에서 보장)
            vbs[0] = vbPos.Get(); strides[0] = sizeof(VERTEX_Pos);

            if (vbNrm) { vbs[1] = vbNrm.Get(); strides[1] = sizeof(VERTEX_Nrm); }
            if (vbUV) { vbs[2] = vbUV.Get();  strides[2] = sizeof(VERTEX_UV); }
            if (vbT) { vbs[3] = vbT.Get();   strides[3] = sizeof(VERTEX_T); }
            if (vbB) { vbs[4] = vbB.Get();   strides[4] = sizeof(VERTEX_B); }

            UINT maxSlot = 0;
            if (vbB)        maxSlot = 4;
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

    std::vector<MeshResource> m_meshes;        // handle -> m_meshes[handle-1]
    std::vector<RenderItem>   m_renderQueue;   // 이번 프레임 제출된 드로우 요청


    struct MaterialResource
    {
        //YunoRenderPass* pass = nullptr;
        RenderPassHandle pass = 0; // handle -> m_passes[pass-1]
    };

    std::vector<YunoMaterial> m_materials;
    MaterialHandle m_defaultMaterial = 0;
    RenderPassHandle m_defaultPass = 0;


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

    // 캐시 조회/생성
    RenderPassHandle GetOrCreatePass(const PassKey& key);

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



private:
    // Debug Grid (Engine-owned)
    bool m_enableDebugGrid = true;

    MeshHandle          m_debugGridMeshHandle = 0;
    MaterialHandle      m_debugGridMaterial = 0;
    RenderPassHandle    m_debugGridPass = 0;

private:
    void CreateDebugGridResources();
    void SubmitDebugGrid();
    MeshHandle CreateLineMesh_PosOnly(const VERTEX_Pos* verts, uint32_t vtxCount);
};
