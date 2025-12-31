#pragma once

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

    // --- Game -> Engine ---
    MeshHandle CreateMesh(const VertexStreams& streams,
        const INDEX* triangles,
        uint32_t triCount) override;

    MaterialHandle CreateMaterial_Default() override;
    MaterialHandle CreateMaterial(const MaterialDesc& desc);

    void Submit(const RenderItem& item) override;
    void Flush() override;                       


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

    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_depthStencilTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

    YunoConstantBuffer<CBDefault> m_cbDefault;
    YunoConstantBuffer<CBMaterial> m_cbMaterial;
    YunoCamera m_camera;
    float m_aspect = 1.0f;



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

            ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
};
