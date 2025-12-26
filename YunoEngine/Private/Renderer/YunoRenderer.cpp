#include "pch.h"

#include "YunoRenderer.h"

// 셰이더
#include "YunoRenderPass.h"
#include "YunoMeshBuffer.h"
#include "YunoShaderCompiler.h"
#include "YunoShader.h"
#include "YunoConstantBuffers.h"

// 인터페이스
#include "IWindow.h"

using Microsoft::WRL::ComPtr;

namespace
{
    void UpdateDynamicCB(ID3D11DeviceContext* ctx, ID3D11Buffer* cb, const void* data, size_t size)
    {
        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(ctx->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        {
            std::memcpy(ms.pData, data, size);
            ctx->Unmap(cb, 0);
        }
    }
}

YunoRenderer::YunoRenderer() = default;
YunoRenderer::~YunoRenderer() = default;

bool YunoRenderer::Initialize(IWindow* window)
{
    if (!window)
        return false;

    HWND hwnd = static_cast<HWND>(window->GetNativeHandle());
    if (!hwnd)
        return false;

    m_width = window->GetClientWidth();
    m_height = window->GetClientHeight();

    if (!CreateDeviceAndSwapChain(hwnd, m_width, m_height))     // 스왑체인 생성
        return false;

    if (!CreateRenderTarget())                              // RT
        return false;

    if (!CreateDepthStencil(m_width, m_height))             // DS
        return false;


    auto CreateDynamicCB = [&](UINT byteWidth, Microsoft::WRL::ComPtr<ID3D11Buffer>& out)
        {
            D3D11_BUFFER_DESC bd{};
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = byteWidth;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            return SUCCEEDED(m_device->CreateBuffer(&bd, nullptr, out.ReleaseAndGetAddressOf()));
        };

    if (!CreateDynamicCB(sizeof(CBDefault), m_cbDefault)) return false;



    m_aspect = (m_height == 0) ? 1.0f : (float)m_width / (float)m_height;
    m_camera.aspect = m_aspect;

    if (!CreateBasicPipeline())
        return false;

    return true;
}

bool YunoRenderer::CreateBasicPipeline()
{
    if (!m_device)
        return false;

    YunoShaderCompiler compiler;

    // 현재 파일 경로 기준으로 Assets/ 아래의 쉐이더를 찾는다.
    const std::filesystem::path shaderPath =
        std::filesystem::path(__FILE__).parent_path().parent_path().parent_path() /
        "Assets" / "Shaders" / "BasicColor.hlsl";

    compiler.AddIncludeDir(shaderPath.parent_path().wstring());

    auto vsBlob = compiler.CompileFromFile(shaderPath.wstring(), "VSMain", "vs_5_0");
    auto psBlob = compiler.CompileFromFile(shaderPath.wstring(), "PSMain", "ps_5_0");

    m_basicVS = std::make_unique<YunoShader>();
    m_basicPS = std::make_unique<YunoShader>();

    if (!m_basicVS->CreateVertexShader(m_device.Get(), vsBlob.Get()))
        return false;
    if (!m_basicPS->CreatePixelShader(m_device.Get(), psBlob.Get()))
        return false;

    static constexpr D3D11_INPUT_ELEMENT_DESC kInputPos[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    m_basicPass = std::make_unique<YunoRenderPass>();

    YunoRenderPassDesc passDesc{};
    passDesc.vs = m_basicVS.get();
    passDesc.ps = m_basicPS.get();
    passDesc.vsBytecode = vsBlob.Get();
    passDesc.inputElements = kInputPos;
    passDesc.inputElementCount = _countof(kInputPos);
    passDesc.enableDepth = true;
    passDesc.cullBack = false;

    if (!m_basicPass->Create(m_device.Get(), passDesc))
        return false;

    m_materials.clear();
    m_defaultMaterial = 0;
    m_defaultMaterial = CreateMaterial_Default();
    return m_defaultMaterial != 0;
}



bool YunoRenderer::CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL outLevel{};

    auto create = [&](UINT createFlags) -> HRESULT
        {
            return D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                createFlags,
                levels,
                1,
                D3D11_SDK_VERSION,
                &sd,
                m_swapChain.GetAddressOf(),
                m_device.GetAddressOf(),
                &outLevel,
                m_context.GetAddressOf()
            );
        };

    HRESULT hr = create(flags);

#if defined(_DEBUG)
    // 디버그 레이어가 설치되어 있지 않으면 실패할 수 있으니 플래그 제거 후 재시도
    if (FAILED(hr) && (flags & D3D11_CREATE_DEVICE_DEBUG))
    {
        flags &= ~D3D11_CREATE_DEVICE_DEBUG;
        hr = create(flags);
    }
#endif

    if (FAILED(hr))
        return false;

#if defined(_DEBUG)
    // InfoQueue: 심각한 D3D 에러가 뜨는 순간 디버거가 멈추게 함
    Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue;
    if (m_device && SUCCEEDED(m_device.As(&infoQueue)))
    {
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);

        // 처음엔 끄는 걸 추천 (경고는 너무 많이 걸릴 수 있음)
        //infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    }
#endif

    return true;
}

bool YunoRenderer::CreateRenderTarget()
{
    ComPtr<ID3D11Texture2D> backBuffer;
    if (FAILED(m_swapChain->GetBuffer(
        0, IID_PPV_ARGS(backBuffer.GetAddressOf()))))
        return false;

    if (FAILED(m_device->CreateRenderTargetView(
        backBuffer.Get(), nullptr, m_backBufferRT.rtv.GetAddressOf())))
        return false;

    return true;
}

bool YunoRenderer::CreateDepthStencil(uint32_t width, uint32_t height)
{
    // 기존 리소스 해제 (Resize 대비)
    m_dsv.Reset();
    m_depthStencilTex.Reset();

    if (width == 0 || height == 0)
        return false;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;

    // 가장 무난한 깊이 포맷 (Depth 24 + Stencil 8)
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    desc.SampleDesc.Count = 1;   // 지금 MSAA 안 쓰는 기준
    desc.SampleDesc.Quality = 0;

    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    HRESULT hr = m_device->CreateTexture2D(&desc, nullptr, m_depthStencilTex.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = desc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = m_device->CreateDepthStencilView(m_depthStencilTex.Get(), &dsvDesc, m_dsv.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}


void YunoRenderer::SetViewPort()
{
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<float>(m_width);
    vp.Height = static_cast<float>(m_height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);
}

void YunoRenderer::ClearDepthStencil()
{
    m_context->ClearDepthStencilView(m_dsv.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,   // 깊이 최댓값으로 초기화
        0       // 스텐실 0
    );
}

void YunoRenderer::BeginFrame()
{
    m_context->OMSetRenderTargets(1, m_backBufferRT.rtv.GetAddressOf(), m_dsv.Get());        // RT ,DS 셋

    SetViewPort();                                                                           // 뷰포트 설정

    const float clearColor[4] = { 0.05f, 0.1f, 0.2f, 1.0f };                                 // 어두운 파랑


    m_context->ClearRenderTargetView(m_backBufferRT.rtv.Get(), clearColor);                  // 화면 클리어

    ClearDepthStencil();                                                                     // 뎁스/스텐실 클리어

}

void YunoRenderer::EndFrame()
{
    // VSync ON
    //m_swapChain->Present(1, 0);
    
    // VSync OFF
    m_swapChain->Present(0, 0);
}

void YunoRenderer::Resize(uint32_t width, uint32_t height)          //기존 RT, DS 없애고 재생성 및 스왑체인 버퍼 크기 리사이즈
{
    if (!m_swapChain || !m_context || !m_device)
        return;

    if (width == 0 || height == 0)
        return;

    if (width == m_width && height == m_height)
        return;

    m_width = width;
    m_height = height;

    m_aspect = (height == 0) ? 1.0f : (float)width / (float)height;
    m_camera.aspect = m_aspect;

    // 1) 바인딩 해제
    ID3D11RenderTargetView* nullRTV[1] = { nullptr };
    m_context->OMSetRenderTargets(1, nullRTV, nullptr);

    // 2) RTV/DSV 리소스 해제
    m_backBufferRT = {};
    m_dsv.Reset();
    m_depthStencilTex.Reset();

    // 3) SwapChain Resize
    HRESULT hr = m_swapChain->ResizeBuffers(
        0,
        m_width,
        m_height,
        DXGI_FORMAT_UNKNOWN,
        0
    );
    if (FAILED(hr))
        return;

    // 4) 새 RTV 생성
    if (!CreateRenderTarget())
        return;

    // 5) 새 DSV 생성
    if (!CreateDepthStencil(m_width, m_height))
        return;
}


#ifdef _DEBUG
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

void YunoRenderer::Shutdown()
{
#if defined(_DEBUG)
    Microsoft::WRL::ComPtr<ID3D11Debug> debug;
    if (m_device) m_device.As(&debug);
#endif

    if (m_context)
    {
        m_context->OMSetRenderTargets(0, nullptr, nullptr);
        m_context->ClearState();
        m_context->Flush();
    }

    // 내 리소스 해제
    m_backBufferRT = {};
    m_dsv.Reset();
    m_depthStencilTex.Reset();
    m_swapChain.Reset();

    // 컨텍스트 먼저
    m_context.Reset();

#if defined(_DEBUG)
    if (debug)
    {
        //debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);                // D3D11 내부 참조까지 전부 출력됨
        debug->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL);         // D3D11 내부 참조는 무시 (의미 없는 경고 출력x)
        debug.Reset();
    }
#endif

    m_device.Reset();

    m_width = 0;
    m_height = 0;
}



// ------------------------------------------------------------
// Game -> Engine API 구현
// ------------------------------------------------------------

MeshHandle YunoRenderer::CreateMesh(const VertexStreams& streams,
    const uint32_t* indices,
    uint32_t indexCount)
{
    auto Has = [&](uint32_t f) { return (streams.flags & f) != 0; };

    if (!m_device)
        return 0;

    // 필수: Position
    if (!Has(VSF_Pos) || streams.pos == nullptr || streams.vtx_count == 0)
        return 0;

    // 인덱스가 있다고 했으면 포인터 필요
    if (indexCount > 0 && indices == nullptr)
        return 0;

    // flags에 포함된 스트림은 반드시 포인터가 있어야 한다
    if (Has(VSF_Nrm) && streams.nrm == nullptr) return 0;
    if (Has(VSF_UV) && streams.uv == nullptr) return 0;
    if (Has(VSF_T) && streams.t == nullptr) return 0;
    if (Has(VSF_B) && streams.b == nullptr) return 0;

    auto CreateVB = [&](const void* data, UINT stride, UINT count,
        ComPtr<ID3D11Buffer>& outVB) -> bool
        {
            D3D11_BUFFER_DESC bd{};
            bd.Usage = D3D11_USAGE_IMMUTABLE;           // 정적 메쉬
            bd.ByteWidth = stride * count;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA init{};
            init.pSysMem = data;

            return SUCCEEDED(m_device->CreateBuffer(&bd, &init, outVB.ReleaseAndGetAddressOf()));
        };

    MeshResource mr{};
    mr.flags = streams.flags;
    mr.vertexCount = streams.vtx_count;
    mr.indexCount = indexCount;

    // 스트림별 VB 생성
    if (!CreateVB(streams.pos, sizeof(VERTEX_Pos), streams.vtx_count, mr.vbPos))
        return 0;

    if (Has(VSF_Nrm))
    {
        if (!CreateVB(streams.nrm, sizeof(VERTEX_Nrm), streams.vtx_count, mr.vbNrm))
            return 0;
    }

    if (Has(VSF_UV))
    {
        if (!CreateVB(streams.uv, sizeof(VERTEX_UV), streams.vtx_count, mr.vbUV))
            return 0;
    }

    if (Has(VSF_T))
    {
        if (!CreateVB(streams.t, sizeof(VERTEX_T), streams.vtx_count, mr.vbT))
            return 0;
    }

    if (Has(VSF_B))
    {
        if (!CreateVB(streams.b, sizeof(VERTEX_B), streams.vtx_count, mr.vbB))
            return 0;
    }

    // 인덱스 버퍼(옵션)
    if (indexCount > 0)
    {
        D3D11_BUFFER_DESC ibd{};
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(uint32_t) * indexCount;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA init{};
        init.pSysMem = indices;

        if (FAILED(m_device->CreateBuffer(&ibd, &init, mr.ib.ReleaseAndGetAddressOf())))
            return 0;
    }

    // 저장하고 핸들 반환 (1-based)
    m_meshes.push_back(std::move(mr));
    return static_cast<MeshHandle>(m_meshes.size());
}

MaterialHandle YunoRenderer::CreateMaterial_Default()
{
    if (!m_basicPass)
        return 0;

    if (m_defaultMaterial != 0)
        return m_defaultMaterial;

    MaterialResource mat{};
    mat.pass = m_basicPass.get();

    m_materials.push_back(mat);
    m_defaultMaterial = static_cast<MaterialHandle>(m_materials.size());
    return m_defaultMaterial;
}

void YunoRenderer::Submit(const RenderItem& item)
{
    if (item.mesh == 0 || item.mesh > m_meshes.size())
        return;

    RenderItem copy = item;
    if (copy.material == 0)
        copy.material = m_defaultMaterial;

    if (copy.material == 0 || copy.material > m_materials.size())
        return;

    m_renderQueue.push_back(copy);
}

void YunoRenderer::Flush()
{
    if (!m_context || !m_cbDefault)
        return;

    for (const RenderItem& item : m_renderQueue)
    {
        if (item.mesh == 0 || item.mesh > m_meshes.size())
            continue;

        const MaterialResource* material = nullptr;
        if (item.material > 0 && item.material <= m_materials.size())
        {
            material = &m_materials[item.material - 1];
        }

        YunoRenderPass* pass = material ? material->pass : nullptr;
        if (!pass)
            pass = m_basicPass.get();
        if (!pass)
            continue;

        pass->Bind(m_context.Get());

        const MeshResource& mr = m_meshes[item.mesh - 1];

        // 슬롯 고정 매핑:
        // 0=Pos, 1=Nrm, 2=UV, 3=T, 4=B
        ID3D11Buffer* vbs[5] = {};
        UINT strides[5] = {};
        UINT offsets[5] = {};

        vbs[0] = mr.vbPos.Get(); strides[0] = sizeof(VERTEX_Pos);
        vbs[1] = mr.vbNrm.Get(); strides[1] = sizeof(VERTEX_Nrm);
        vbs[2] = mr.vbUV.Get();  strides[2] = sizeof(VERTEX_UV);
        vbs[3] = mr.vbT.Get();   strides[3] = sizeof(VERTEX_T);
        vbs[4] = mr.vbB.Get();   strides[4] = sizeof(VERTEX_B);

        // 가장 높은 사용 슬롯까지 바인딩 (중간 슬롯이 nullptr이어도 count 안에 포함될 수 있음)
        UINT maxSlot = 0;
        if (mr.vbB)       maxSlot = 4;
        else if (mr.vbT)  maxSlot = 3;
        else if (mr.vbUV) maxSlot = 2;
        else if (mr.vbNrm)maxSlot = 1;
        else              maxSlot = 0;

        m_context->IASetVertexBuffers(0, maxSlot + 1, vbs, strides, offsets);

        if (mr.ib && mr.indexCount > 0)
            m_context->IASetIndexBuffer(mr.ib.Get(), DXGI_FORMAT_R32_UINT, 0);

        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        using namespace DirectX;

        XMMATRIX W = XMLoadFloat4x4(&item.world);
        XMMATRIX V = m_camera.View();
        XMMATRIX P = m_camera.Proj();
        XMMATRIX WVP = W * V * P;

        CBDefault cbd{};
        XMStoreFloat4x4(&cbd.mWorld, XMMatrixTranspose(W));
        XMStoreFloat4x4(&cbd.mView, XMMatrixTranspose(V));
        XMStoreFloat4x4(&cbd.mProj, XMMatrixTranspose(P));
        XMStoreFloat4x4(&cbd.mWVP, XMMatrixTranspose(WVP));

        UpdateDynamicCB(m_context.Get(), m_cbDefault.Get(), &cbd, sizeof(cbd));
        ID3D11Buffer* cbs[] = { m_cbDefault.Get() };
        m_context->VSSetConstantBuffers(0, 1, cbs);

        if (mr.ib && mr.indexCount > 0)
            m_context->DrawIndexed(mr.indexCount, 0, 0);
        else
            m_context->Draw(mr.vertexCount, 0);
    }

    m_renderQueue.clear();
}