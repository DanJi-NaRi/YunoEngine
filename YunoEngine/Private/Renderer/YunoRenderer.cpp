#include "pch.h"

#include "YunoRenderer.h"

// 셰이더
#include "YunoRenderPass.h"
//#include "YunoMeshBuffer.h"
#include "YunoShaderCompiler.h"
#include "YunoShader.h"


//imgui
#include "ImGuiManager.h"
#include "UImgui.h"

// 인터페이스
#include "IWindow.h"
#include "YunoEngine.h"

using Microsoft::WRL::ComPtr;


YunoRenderer::YunoRenderer() = default;
YunoRenderer::~YunoRenderer() = default;


// assert(Has(VSF_Pos) && streams.pos != nullptr && streams.vtx_count != 0);
// 어썰트 나중에 싹 바꾸기 ㄱㄱ 필수임 개좋음!!

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

    CheckMSAA();                                            // MSAA 가능 체크

    if (!SetMSAASamples(m_msaaSamples))
        return false;

    if (!CreateRenderTarget())                              // RT
        return false;

    if (!CreateDepthStencil(m_width, m_height))             // DS
        return false;

    if (!CreateMSAARenderTarget(m_width, m_height))         // MSAA RT
        return false;

    if (!CreateMSAADepthStencil(m_width, m_height))         // MSAA DS
        return false;

    // 상수 버퍼 생성
    if (!m_cbFrame.Create(m_device.Get())) return false;
    if (!m_cbObject_Matrix.Create(m_device.Get())) return false;
    if (!m_cbObject_Material.Create(m_device.Get())) return false; 
    if (!m_cbLight.Create(m_device.Get())) return false;
    if (!CreatePPCB()) return false;


    m_aspect = (m_height == 0) ? 1.0f : (float)m_width / (float)m_height;
    m_camera.aspect = m_aspect;

    // 디폴트 텍스쳐, 샘플러 생성
    if (!CreateDefaultTextures()) return false;
    if (!CreateSamplerPresets()) return false;

    // 쉐이더들 초기화
    if (!CreateShaders()) return false;

    // 디폴트 파이프라인 생성
    if (!CreateBasicPipeline()) return false;

    if (m_defaultMaterial == 0)
        m_defaultMaterial = CreateMaterial_Default();
    if (m_defaultMaterial == 0)     // 생성 실패하면 리턴
        return false;

    if (!CreatePPRenderTarget(m_width, m_height)) return false;
    if (!CreatePPMaterial()) return false;
    SetPP_Pass();

    if (!CreateShadowMap(4096, 4096)) return false;
    InitShadowPass();

#ifdef _DEBUG
    // 디버그 리소스 생성
    CreateDebugGridResources();
    CreateDebugMeshResources();
#endif


    return true;
}

bool YunoRenderer::CreateShaders()
{
    // 여기서 쉐이더들 초기화 쭉 하면 됨
    if (!LoadShader(ShaderId::Basic, "../Assets/Shaders/BasicColor.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PBRBase, "../Assets/Shaders/PBR_Base.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::BasicAnimation, "../Assets/Shaders/BasicAnimation.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PBRAnimation, "../Assets/Shaders/PBR_Animation.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::UIBase, "../Assets/Shaders/UI_Base.hlsl", "VSMain", "PSMain")) return false;

    if (!CreatePPShader()) return false;

    //Debug
    if (!LoadShader(ShaderId::DebugGrid, "../Assets/Shaders/DebugGrid.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::DebugMesh, "../Assets/Shaders/DebugMesh.hlsl", "VSMain", "PSMain")) return false;

    //ps 안씀
    if (!LoadShader(ShaderId::ShadowPass, "../Assets/Shaders/ShadowMapWrite.hlsl", "VSMain", "PSMain")) return false;

    //픽셀셰이더 필요없을 때 쓰는 셰이더(섀도우맵)
    YunoShader empty;
    empty.CreateEmptyShader();
    ShaderProgram sp{};
    sp.vs = std::make_unique<YunoShader>(empty);
    sp.ps = std::make_unique<YunoShader>(empty);
    sp.vsBytecode = nullptr;
    m_programs[ShaderId::None] = std::move(sp);

    return true;
}

bool YunoRenderer::CreateBasicPipeline()
{
    if (!m_device)
        return false;

    m_materials.clear();
    m_defaultMaterial = 0;
    m_defaultMaterial = CreateMaterial_Default();
    return m_defaultMaterial != 0;
}



// ------------------------------------------------------------
// Renderer
// ------------------------------------------------------------

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
    m_backBufferTex.Reset();
    m_backBufferRT = {};

    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_backBufferTex.GetAddressOf()))))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    if (FAILED(m_device->CreateRenderTargetView(
        m_backBufferTex.Get(), &rtvDesc, m_backBufferRT.rtv.GetAddressOf())))
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

    desc.SampleDesc.Count = 1;
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

bool YunoRenderer::CreateMSAARenderTarget(uint32_t width, uint32_t height)
{
    m_msaaRT = {};

    if (m_msaaSamples <= 1)
        return true; // MSAA off면 만들 필요 없음

    D3D11_TEXTURE2D_DESC td{};
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    td.SampleDesc.Count = m_msaaSamples;
    td.SampleDesc.Quality = m_msaaQuality;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    if (FAILED(m_device->CreateTexture2D(&td, nullptr, m_msaaRT.tex.GetAddressOf())))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = td.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    rtvDesc.Texture2D.MipSlice = 0;

    if (FAILED(m_device->CreateRenderTargetView(m_msaaRT.tex.Get(), &rtvDesc, m_msaaRT.rtv.GetAddressOf())))
        return false;

    m_msaaRT.w = width;
    m_msaaRT.h = height;
    m_msaaRT.fmt = td.Format;

    // MSAA 텍스처는 보통 SRV가 필요 없다(Resolve 후 백버퍼로)
    // 필요해지면 별도의 non-MSAA 텍스처로 Resolve해서 SRV 만드는 구조로 확장.
    return true;
}

bool YunoRenderer::CreateMSAADepthStencil(uint32_t width, uint32_t height)
{
    m_msaaDepthTex.Reset();
    m_msaaDSV.Reset();

    if (m_msaaSamples <= 1)
        return true;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = m_msaaSamples;
    desc.SampleDesc.Quality = m_msaaQuality;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    if (FAILED(m_device->CreateTexture2D(&desc, nullptr, m_msaaDepthTex.GetAddressOf())))
        return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = desc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    dsvDesc.Texture2D.MipSlice = 0;

    if (FAILED(m_device->CreateDepthStencilView(m_msaaDepthTex.Get(), &dsvDesc, m_msaaDSV.GetAddressOf())))
        return false;

    return true;
}

bool YunoRenderer::CheckMSAA()
{
    if (m_msaaSamples > 1)
    {
        UINT qColor = 0;
        UINT qDepth = 0;

        const DXGI_FORMAT colorFmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
        const DXGI_FORMAT depthFmt = DXGI_FORMAT_D24_UNORM_S8_UINT;

        const bool okColor = SUCCEEDED(m_device->CheckMultisampleQualityLevels(colorFmt, m_msaaSamples, &qColor)) && (qColor > 0);
        const bool okDepth = SUCCEEDED(m_device->CheckMultisampleQualityLevels(depthFmt, m_msaaSamples, &qDepth)) && (qDepth > 0);

        if (!okColor || !okDepth)
        {
            m_msaaSamples = 1;
            m_msaaQuality = 0;
            std::cout << "[MSAA] Not supported for requested sample count. Fallback to 1.\n";
            return false;
        }
        else
        {
            const UINT qCommon = (qColor < qDepth) ? qColor : qDepth;
            m_msaaQuality = qCommon - 1;

            //std::cout << "[MSAA] Samples=" << m_msaaSamples
            //    << " Quality(Color)=" << (qColor - 1)
            //    << " Quality(Depth)=" << (qDepth - 1)
            //    << " Using=" << m_msaaQuality << "\n";
        }
    }
    else
    {
        m_msaaQuality = 0;
    }
    return true;
}

bool YunoRenderer::CreateShadowMap(uint32_t width, uint32_t height)
{
    m_ShadowMap.width = width;
    m_ShadowMap.height = height;
    m_ShadowMap.texFormat = DXGI_FORMAT_R32_TYPELESS;
    m_ShadowMap.dsvFormat = DXGI_FORMAT_D32_FLOAT;
    m_ShadowMap.srvFormat = DXGI_FORMAT_R32_FLOAT;

    m_shadowInfo.shadowMapSize = width;
    m_shadowBias = 0.0005f;

    m_cbShadow.Create(m_device.Get());

    D3D11_TEXTURE2D_DESC td{};
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = m_ShadowMap.texFormat;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    if (FAILED(m_device->CreateTexture2D(&td, nullptr, m_ShadowMap.dstex.GetAddressOf())))
        return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd{};
    dsvd.Format = m_ShadowMap.dsvFormat;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0;

    if (FAILED(m_device->CreateDepthStencilView(m_ShadowMap.dstex.Get(), &dsvd, m_ShadowMap.dsv.GetAddressOf())))
        return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    if (FAILED(m_device->CreateShaderResourceView(m_ShadowMap.dstex.Get(), &srvDesc, m_ShadowMap.dssrv.GetAddressOf())))
        return false;

    m_shadowCamera.position = XMFLOAT3(0, 20, 0);
    m_shadowCamera.target = XMFLOAT3(0, 0, -0.01f);
    m_shadowCamera.up = XMFLOAT3(0, 1, 0);

    return true;
}

void YunoRenderer::InitShadowPass()
{
    m_ShadowPassKey.vs = ShaderId::ShadowPass;
    m_ShadowPassKey.ps = ShaderId::None;
    m_ShadowPassKey.vertexFlags = VSF_Pos;
    m_ShadowPassKey.blend = BlendPreset::Opaque;
    m_ShadowPassKey.raster = RasterPreset::CullFront;
    m_ShadowPassKey.depth = DepthPreset::ReadWrite;

    m_ShadowPass = GetOrCreatePass(m_ShadowPassKey);
}

void YunoRenderer::DrawShadowMap()
{
    ID3D11RenderTargetView* curRTV;
    ID3D11DepthStencilView* curDSV;
    m_context->OMGetRenderTargets(1, &curRTV, &curDSV);

    //ViewProj Bind
    XMMATRIX LightView = m_shadowCamera.View();
    XMMATRIX LightProj = m_shadowCamera.ProjShadowOrtho(100, 100);

    XMStoreFloat4x4(&m_shadowInfo.lightViewProj, XMMatrixTranspose(LightView * LightProj));
    m_shadowInfo.shadowBias = m_shadowBias;

    m_cbShadow.Update(m_context.Get(), m_shadowInfo);

    UnBindAllSRV();
        
    m_context->ClearDepthStencilView(m_ShadowMap.dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    ID3D11RenderTargetView* nullRTV[1] = { nullptr };
    m_context->OMSetRenderTargets(0, nullRTV, m_ShadowMap.dsv.Get());
    SetViewPort(m_ShadowMap.width, m_ShadowMap.height);
    
    if (m_ShadowPass == 0 || m_ShadowPass > m_passes.size())
        return;

    // 렌더 패스 바인드
    m_passes[m_ShadowPass - 1]->Bind(m_context.Get());

    ID3D11Buffer* cb = m_cbShadow.Get();

    m_context->VSSetConstantBuffers(4, 1, &cb);
    //m_context->PSSetConstantBuffers(4, 1, &cb);

    for (const RenderItem& item : m_renderQueue)
    {
        if (!item.castShadow)
            continue;

        if (item.meshHandle == 0 || item.meshHandle > m_meshes.size())
            continue;

        const YunoMaterial* material = nullptr;

        if (item.materialHandle > 0 && item.materialHandle <= m_materials.size()) // 핸들 유효성 체크
            material = &m_materials[item.materialHandle - 1];

        if (!material)
        {
            if (m_defaultMaterial > 0 && m_defaultMaterial <= m_materials.size())
                material = &m_materials[m_defaultMaterial - 1];
        }

        // 메쉬 바인드
        const MeshResource& mesh = m_meshes[item.meshHandle - 1];
        mesh.Bind(m_context.Get());

        // 상수 버퍼 바인드
        CBPerObject_Matrix cbPerObject_matrix{};

        XMMATRIX W = XMLoadFloat4x4(&item.Constant.world);

        XMMATRIX mAnim[MAX_BONE];
        if (item.haveAnim)
        {
            size_t i = 0;
            for (auto& animTM : item.Constant.boneAnim)
            {
                mAnim[i] = XMLoadFloat4x4(&animTM);
                i++;
            }
        }

        XMStoreFloat4x4(&cbPerObject_matrix.mWorld, XMMatrixTranspose(W));

        if (item.haveAnim)
        {
            size_t i = 0;
            for (auto& animTM : mAnim)
            {
                XMStoreFloat4x4(&cbPerObject_matrix.mBoneAnim[i], XMMatrixTranspose(animTM));
                i++;
            }
        }

        m_cbObject_Matrix.Update(m_context.Get(), cbPerObject_matrix);

        ID3D11Buffer* cbPerObj_Matrix_Buffers[] = { m_cbObject_Matrix.Get() };
        m_context->VSSetConstantBuffers(0, 1, cbPerObj_Matrix_Buffers);

        // 드로우
        if (mesh.ib && mesh.indexCount > 0)
            m_context->DrawIndexed(mesh.indexCount, 0, 0);
        else
            m_context->Draw(mesh.vertexCount, 0);
    }

    m_context->OMSetRenderTargets(1, &curRTV, curDSV);
    SetViewPort();
    curRTV->Release();
    curDSV->Release();
}

RenderTarget& YunoRenderer::CurPostRT()
{
    const float clearColor[4] = { 0.05f, 0.1f, 0.2f, 1.0f };

    auto& dstRT = m_postRT[m_postIndex & 1];

    m_context->ClearRenderTargetView(dstRT.rtv.Get(), clearColor);

    return dstRT;
}

RenderTarget& YunoRenderer::NextPostRT()
{
    const float clearColor[4] = { 0.05f, 0.1f, 0.2f, 1.0f };

    auto& dstRT = m_postRT[(m_postIndex + 1) & 1];

    m_context->ClearRenderTargetView(dstRT.rtv.Get(), clearColor);

    return dstRT;
}

bool YunoRenderer::CreatePPRenderTarget(uint32_t width, uint32_t height)
{
    if (!CreateDefaultPPRT(width, height)) return false;
    if (!CreateBloomRT(width, height)) return false;

    return true;
}

bool YunoRenderer::CreateDefaultPPRT(uint32_t width, uint32_t height)
{
    m_sceneRT = {};
    m_postRT[0] = {};
    m_postRT[1] = {};

    D3D11_TEXTURE2D_DESC td{};
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    if (FAILED(m_device->CreateTexture2D(&td, nullptr, m_sceneRT.tex.GetAddressOf())))
        return false;

    if (FAILED(m_device->CreateTexture2D(&td, nullptr, m_postRT[0].tex.GetAddressOf())))
        return false;

    if (FAILED(m_device->CreateTexture2D(&td, nullptr, m_postRT[1].tex.GetAddressOf())))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = td.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    if (FAILED(m_device->CreateRenderTargetView(m_sceneRT.tex.Get(), &rtvDesc, m_sceneRT.rtv.GetAddressOf())))
        return false;

    if (FAILED(m_device->CreateRenderTargetView(m_postRT[0].tex.Get(), &rtvDesc, m_postRT[0].rtv.GetAddressOf())))
        return false;

    if (FAILED(m_device->CreateRenderTargetView(m_postRT[1].tex.Get(), &rtvDesc, m_postRT[1].rtv.GetAddressOf())))
        return false;

    //셰이더리소스뷰 정보 구성.
    D3D11_SHADER_RESOURCE_VIEW_DESC sd = {};
    ZeroMemory(&sd, sizeof(sd));
    sd.Format = td.Format;										//텍스처와 동일포멧유지.
    sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    sd.Texture2D.MipLevels = td.MipLevels;
    sd.Texture2D.MostDetailedMip = 0;

    if (FAILED(m_device->CreateShaderResourceView(m_sceneRT.tex.Get(), &sd, m_sceneRT.srv.GetAddressOf())))
        return false;
    if (FAILED(m_device->CreateShaderResourceView(m_postRT[0].tex.Get(), &sd, m_postRT[0].srv.GetAddressOf())))
        return false;
    if (FAILED(m_device->CreateShaderResourceView(m_postRT[1].tex.Get(), &sd, m_postRT[1].srv.GetAddressOf())))
        return false;

    m_sceneRT.w = width;
    m_sceneRT.h = height;
    m_sceneRT.fmt = td.Format;

    m_postRT[0].w = width;
    m_postRT[0].h = height;
    m_postRT[0].fmt = td.Format;

    m_postRT[1].w = width;
    m_postRT[1].h = height;
    m_postRT[1].fmt = td.Format;

    return true;
}

bool YunoRenderer::CreateBloomRT(uint32_t width, uint32_t height)
{
    D3D11_TEXTURE2D_DESC td{};
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = td.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    //셰이더리소스뷰 정보 구성.
    D3D11_SHADER_RESOURCE_VIEW_DESC sd = {};
    sd.Format = td.Format;										//텍스처와 동일포멧유지.
    sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    sd.Texture2D.MipLevels = td.MipLevels;
    sd.Texture2D.MostDetailedMip = 0;

    for (int i = 0; i < 4; ++i)
    {
        m_bloomRT[i] = {};

        uint32_t scale = 1u << (i + 1); //2의 제곱씩 증가
        td.Width = std::max(1u, width / scale);
        td.Height = std::max(1u, height / scale);

        //CreateTexture + RTV + SRV
        if (FAILED(m_device->CreateTexture2D(&td, nullptr, m_bloomRT[i].tex.GetAddressOf())))
            return false;
        if (FAILED(m_device->CreateTexture2D(&td, nullptr, m_blurTemp[i].tex.GetAddressOf())))
            return false;
        if (FAILED(m_device->CreateRenderTargetView(m_bloomRT[i].tex.Get(), &rtvDesc, m_bloomRT[i].rtv.GetAddressOf())))
            return false;
        if (FAILED(m_device->CreateRenderTargetView(m_blurTemp[i].tex.Get(), &rtvDesc, m_blurTemp[i].rtv.GetAddressOf())))
            return false;
        if (FAILED(m_device->CreateShaderResourceView(m_bloomRT[i].tex.Get(), &sd, m_bloomRT[i].srv.GetAddressOf())))
            return false;
        if (FAILED(m_device->CreateShaderResourceView(m_blurTemp[i].tex.Get(), &sd, m_blurTemp[i].srv.GetAddressOf())))
            return false;
        m_bloomRT[i].w = td.Width;
        m_bloomRT[i].h = td.Height;
        m_bloomRT[i].fmt = td.Format;
        m_blurTemp[i].w = td.Width;
        m_blurTemp[i].h = td.Height;
        m_blurTemp[i].fmt = td.Format;
    }

    return true;
}

bool YunoRenderer::CreatePPShader()
{
    if (!LoadShader(ShaderId::PP_Default, "../Assets/Shaders/PP_Default.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PP_Threshold, "../Assets/Shaders/PP_Threshold.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PP_DownSample, "../Assets/Shaders/PP_DownSample.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PP_BlurH, "../Assets/Shaders/PP_BlurHorizon.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PP_BlurV, "../Assets/Shaders/PP_BlurVertical.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PP_Combine, "../Assets/Shaders/PP_Combine.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::PP_ToneMap, "../Assets/Shaders/PP_ToneMap.hlsl", "VSMain", "PSMain")) return false;

    return true;
}

bool YunoRenderer::CreatePPMaterial()
{
    //Default
    MaterialDesc md{};
    md.passKey.vs = ShaderId::PP_Default;
    md.passKey.ps = ShaderId::PP_Default;
    md.passKey.vertexFlags = 0;
    md.passKey.blend = BlendPreset::Opaque;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::Off;
    md.passKey.domain = MaterialDomain::PostProcess;

    MaterialHandle h = CreateMaterial(md);
    if (h)
    {
        m_PPMaterials.emplace(PostProcessFlag::Default, h);
        m_ppDefaultMat = h;
    }
    else
        return false;

    //Threshold
    md.passKey.vs = ShaderId::PP_Threshold;
    md.passKey.ps = ShaderId::PP_Threshold;
    h = CreateMaterial(md);
    if (h)
        m_ppThresholdMat = h;
    else
        return false;

    //DownSampling
    md.passKey.vs = ShaderId::PP_DownSample;
    md.passKey.ps = ShaderId::PP_DownSample;
    h = CreateMaterial(md);
    if (h)
        m_ppDownSampleMat = h;
    else
        return false;

    //BlurH
    md.passKey.vs = ShaderId::PP_BlurH;
    md.passKey.ps = ShaderId::PP_BlurH;
    h = CreateMaterial(md);
    if (h)
        m_ppBlurHMat = h;
    else
        return false;

    //BlurV
    md.passKey.vs = ShaderId::PP_BlurV;
    md.passKey.ps = ShaderId::PP_BlurV;
    h = CreateMaterial(md);
    if (h)
        m_ppBlurVMat = h;
    else
        return false;

    //Combine
    md.passKey.vs = ShaderId::PP_Combine;
    md.passKey.ps = ShaderId::PP_Combine;
    h = CreateMaterial(md);
    if (h)
        m_ppCombineMat = h;
    else
        return false;

    //ToneMapping
    md.passKey.vs = ShaderId::PP_ToneMap;
    md.passKey.ps = ShaderId::PP_ToneMap;
    h = CreateMaterial(md);
    if (h)
        m_ppToneMapMat = h;
    else
        return false;

    return true;
}

bool YunoRenderer::CreatePPCB()
{
    if(!m_ppCB.Create(m_device.Get())) return false;
    if(!m_ppCBloom.Create(m_device.Get())) return false;

    return true;
}

void YunoRenderer::SetPP_Pass()
{
    if (m_PPFlag == 0)
    {
        m_PPFlag = PostProcessFlag::Default
                            | PostProcessFlag::Bloom;
    }

    m_ppChain.clear();

    if (PostProcessFlag::Default & m_PPFlag)
    {
        PostProcessPass pp = {};
        pp.enabled = true;
        auto it = m_PPMaterials.find(PostProcessFlag::Default);
        if(it != m_PPMaterials.end())
            pp.material = it->second;
        pp.rtIdx = 0;
        m_ppChain.push_back(pp);
    }

    if (PostProcessFlag::Bloom & m_PPFlag)
    {
        PostProcessPass pp = {};
        pp.enabled = true;
        auto it = m_PPMaterials.find(PostProcessFlag::Bloom);
        if (it != m_PPMaterials.end())
            pp.material = it->second;
        pp.rtIdx = 0;
    }
}

void YunoRenderer::PostProcess()
{
    if (m_msaaSamples > 1 && m_msaaRT.tex && m_backBufferTex)
    {
        m_context->ResolveSubresource(
            m_sceneRT.tex.Get(), 0,
            m_msaaRT.tex.Get(), 0,
            DXGI_FORMAT_R32G32B32A32_FLOAT);
    }

    BindSamplers();

    auto input = PostProcessScene();
    if(m_PPFlag & PostProcessFlag::Bloom)
        input = PostProcessBloom(input);
    PostProcessFinal(input);
}

ID3D11ShaderResourceView* YunoRenderer::PostProcessScene()
{
    ID3D11ShaderResourceView* input = m_sceneRT.srv.Get();
    //ID3D11ShaderResourceView* input = m_ShadowMap.dssrv.Get();

    for (auto& pp : m_ppChain)
    {
        if (!pp.enabled) continue;

        auto& dst = NextPostRT();
        UnBindAllSRV();
        BindRT(dst.rtv.Get());

        const YunoMaterial* material = nullptr;

        if (pp.material > 0 && pp.material <= m_materials.size()) //핸들 유효성 체크
            material = &m_materials[pp.material - 1];

        if (!material)
            continue;

        RenderPassHandle passHandle = material->pass;

        if (passHandle == 0 || passHandle > m_passes.size())
            continue;

        m_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
        m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 렌더 패스 바인드
        m_passes[passHandle - 1]->Bind(m_context.Get());

        // 텍스쳐 바인드
        m_context->PSSetShaderResources(0, 1, &input);

        // 드로우
        m_context->Draw(3, 0);

        input = dst.srv.Get();
        m_postIndex++;
    }

    return input;
}

ID3D11ShaderResourceView* YunoRenderer::PostProcessBloom(ID3D11ShaderResourceView* input)
{
    auto bloominput = input;

    UnBindAllSRV();
    BindRT(m_bloomRT[0].rtv.Get());
    SetViewPort(m_bloomRT[0].w, m_bloomRT[0].h);
    //Threshold(밝은 값만 추출 + 1 / 2샘플링)
    CBPostProcess cbpp;
    cbpp.InvScreenSize = { (float)1 / m_bloomRT[0].w, (float)1 / m_bloomRT[0].h };
    cbpp.threshold = m_Threshold;
    m_ppCB.Update(m_context.Get(), cbpp);
    BindBloomThreshold(input);

    for (int i = 1; i < 3; i++)
    {
        UnBindAllSRV();
        BindRT(m_bloomRT[i].rtv.Get());
        SetViewPort(m_bloomRT[i].w, m_bloomRT[i].h);
        //DownSampling
        m_ppCB.Update(m_context.Get(), { {(float)1 / m_bloomRT[i - 1].w, (float)1 / m_bloomRT[i - 1].h} });
        BindBloomDownSample(m_bloomRT[i - 1].srv.Get());
    }

    for (int i = 3; i > -1; i--)
    {
        UnBindAllSRV();
        BindRT(m_blurTemp[i].rtv.Get());
        SetViewPort(m_blurTemp[i].w, m_blurTemp[i].h);
        //Blur
        m_ppCB.Update(m_context.Get(), { {(float)1 / m_bloomRT[i].w, (float)1 / m_bloomRT[i].h}, 2.0f, 0 });
        // 텍스쳐 바인드
        //BlurH(rt = blurtemp, srv = bloomRT)
        BindBloomBlurH(m_bloomRT[i].srv.Get());
        //BlurV(rt = bloomRT, srv = blurtemp)
        UnBindAllSRV();
        BindRT(m_bloomRT[i].rtv.Get());
        BindBloomBlurV(m_blurTemp[i].srv.Get());
    }

    auto& dst = NextPostRT();
    UnBindAllSRV();
    BindRT(dst.rtv.Get());
    SetViewPort();
    BindBloomCombine(bloominput);

    bloominput = dst.srv.Get();
    m_postIndex++;
   
    return bloominput;
}

void YunoRenderer::BindBloomThreshold(ID3D11ShaderResourceView* input)
{
    if (m_ppThresholdMat <= 0 || m_ppThresholdMat > m_materials.size())
        return;

    const YunoMaterial& material = m_materials[m_ppThresholdMat - 1];

    RenderPassHandle passHandle = material.pass;

    if (passHandle == 0 || passHandle > m_passes.size())
        return;

    // 렌더 패스 바인드
    m_passes[passHandle - 1]->Bind(m_context.Get());

    // 텍스쳐 바인드
    ID3D11Buffer* cb = m_ppCB.Get();
    m_context->PSSetConstantBuffers(0, 1, &cb);
    m_context->PSSetShaderResources(0, 1, &input);

    // 드로우
    m_context->Draw(3, 0);
}

void YunoRenderer::BindBloomDownSample(ID3D11ShaderResourceView* input)
{
    if (m_ppDownSampleMat <= 0 || m_ppDownSampleMat > m_materials.size())
        return;

    const YunoMaterial& material = m_materials[m_ppDownSampleMat - 1];

    RenderPassHandle passHandle = material.pass;

    if (passHandle == 0 || passHandle > m_passes.size())
        return;

    // 렌더 패스 바인드
    m_passes[passHandle - 1]->Bind(m_context.Get());

    // 텍스쳐 바인드
    ID3D11Buffer* cb = m_ppCB.Get();
    m_context->PSSetConstantBuffers(0, 1, &cb);
    m_context->PSSetShaderResources(0, 1, &input);

    // 드로우
    m_context->Draw(3, 0);
}

void YunoRenderer::BindBloomBlurH(ID3D11ShaderResourceView* input)
{
    if (m_ppBlurHMat <= 0 || m_ppBlurHMat > m_materials.size())
        return;

    const YunoMaterial& materialH = m_materials[m_ppBlurHMat - 1];

    RenderPassHandle passHandle = materialH.pass;

    if (passHandle == 0 || passHandle > m_passes.size())
        return;

    // 렌더 패스 바인드
    m_passes[passHandle - 1]->Bind(m_context.Get());

    ID3D11Buffer* cb = m_ppCB.Get();
    m_context->PSSetConstantBuffers(0, 1, &cb);
    m_context->PSSetShaderResources(0, 1, &input);

    // 드로우
    m_context->Draw(3, 0);
}

void YunoRenderer::BindBloomBlurV(ID3D11ShaderResourceView* input)
{
    if (m_ppBlurVMat <= 0 || m_ppBlurVMat > m_materials.size())
        return;

    const YunoMaterial& materialH = m_materials[m_ppBlurVMat - 1];

    RenderPassHandle passHandle = materialH.pass;

    if (passHandle == 0 || passHandle > m_passes.size())
        return;

    // 렌더 패스 바인드
    m_passes[passHandle - 1]->Bind(m_context.Get());

    ID3D11Buffer* cb = m_ppCB.Get();
    m_context->PSSetConstantBuffers(0, 1, &cb);
    m_context->PSSetShaderResources(0, 1, &input);

    // 드로우
    m_context->Draw(3, 0);
}

void YunoRenderer::BindBloomCombine(ID3D11ShaderResourceView* input)
{
    if (m_ppCombineMat <= 0 || m_ppCombineMat > m_materials.size())
        return;

    const YunoMaterial& materialH = m_materials[m_ppCombineMat - 1];

    RenderPassHandle passHandle = materialH.pass;

    if (passHandle == 0 || passHandle > m_passes.size())
        return;

    // 렌더 패스 바인드
    m_passes[passHandle - 1]->Bind(m_context.Get());

    m_ppCBloom.Update(m_context.Get(), { XMFLOAT4(0.4, 0.3f, 0.2f, 0.1f) , m_BloomIntensity });
    // 텍스쳐 바인드
    ID3D11Buffer* cb = m_ppCBloom.Get();
    m_context->PSSetConstantBuffers(1, 1, &cb);

    m_context->PSSetShaderResources(0, 1, &input);
    ID3D11ShaderResourceView* bloomRT[4] = { m_bloomRT[0].srv.Get(), m_bloomRT[1].srv.Get(), m_bloomRT[2].srv.Get(), m_bloomRT[3].srv.Get() };
    m_context->PSSetShaderResources(2, 4, bloomRT);

    // 드로우
    m_context->Draw(3, 0);
}

void YunoRenderer::PostProcessFinal(ID3D11ShaderResourceView* input)
{
    UnBindAllSRV();
    BindRT(m_backBufferRT.rtv.Get());

    const YunoMaterial* material = nullptr;

    if (m_ppToneMapMat > 0 && m_ppToneMapMat <= m_materials.size()) // 핸들 유효성 체크
        material = &m_materials[m_ppToneMapMat - 1];
        //material = &m_materials[m_ppDefaultMat - 1];

    if (!material)
        return;

    RenderPassHandle passHandle = material->pass;

    CBPostProcess cbpp{};
    cbpp.exposure = m_Exposure;
    m_ppCB.Update(m_context.Get(), cbpp);
    auto cb = m_ppCB.Get();
    m_context->PSSetConstantBuffers(0, 1, &cb);

    // 렌더 패스 바인드
    m_passes[passHandle - 1]->Bind(m_context.Get());

    // 텍스쳐 바인드
    m_context->PSSetShaderResources(0, 1, &input);

    // 드로우
    m_context->Draw(3, 0);
}

void YunoRenderer::UnBindAllSRV()
{
    ID3D11ShaderResourceView* nullSRV[16] = {}; //이전에 바인드된 srv가 이제 set됄 rtv가 되는것을 방지->rtv를 동시에 텍스쳐와 렌더타겟으로 사용방지
    m_context->PSSetShaderResources(0, 16, nullSRV);
}

void YunoRenderer::BindRT(ID3D11RenderTargetView* rt)
{
    m_context->OMSetRenderTargets(1, &rt, nullptr);
    //SetViewPort();
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

void YunoRenderer::SetViewPort(uint32_t width, uint32_t height)
{
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);
}

void YunoRenderer::ClearDepthStencil()
{
    if (m_msaaSamples > 1 && m_msaaDSV)
        m_context->ClearDepthStencilView(m_msaaDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    else
        m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void YunoRenderer::BeginFrame()
{
    // RT ,DS 셋
    if (m_msaaSamples > 1 && m_msaaRT.rtv && m_msaaDSV)     // MSAA 사용
    {
        m_context->OMSetRenderTargets(1, m_msaaRT.rtv.GetAddressOf(), m_msaaDSV.Get());
    }
    else                                                    // MSAA 미사용
    {
        m_context->OMSetRenderTargets(1, m_sceneRT.rtv.GetAddressOf(), m_dsv.Get());
    }        

    SetViewPort();                                                                           // 뷰포트 설정

    const float clearColor[4] = { 0.05f, 0.1f, 0.2f, 1.0f };                                 // 어두운 파랑

    // 화면 클리어
    if (m_msaaSamples > 1 && m_msaaRT.rtv)                  // MSAA 사용
        m_context->ClearRenderTargetView(m_msaaRT.rtv.Get(), clearColor);
    else                                                    // MSAA 미사용
        m_context->ClearRenderTargetView(m_sceneRT.rtv.Get(), clearColor);

    m_context->ClearRenderTargetView(m_backBufferRT.rtv.Get(), clearColor);

    // 뎁스/스텐실 클리어
    ClearDepthStencil();                                                                     

}

void YunoRenderer::EndFrame()
{
    

    // VSync가 뭔지는 다들 알죠? >> 화면 주사율이랑 게임 프레임이랑 동기화 시키는 겁니다
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
    m_backBufferTex.Reset();
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

    if (!CreateRenderTarget())
        return;

    if (!CreateDepthStencil(m_width, m_height))
        return;

    if (!CreateMSAARenderTarget(m_width, m_height))
        return;

    if (!CreateMSAADepthStencil(m_width, m_height))
        return;

    if (!CreatePPRenderTarget(m_width, m_height))
        return;
}

bool YunoRenderer::SetMSAASamples(uint32_t samples)
{
    if (!m_device)
        return false;

    if (!(samples == 1 || samples == 2 || samples == 4 || samples == 8))
        return false;

    if (samples == m_msaaSamples)
        return true;

    uint32_t newSamples = samples;
    uint32_t newQuality = 0;

    if (newSamples > 1)
    {
        UINT qColor = 0;
        UINT qDepth = 0;

        const DXGI_FORMAT colorFmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
        const DXGI_FORMAT depthFmt = DXGI_FORMAT_D24_UNORM_S8_UINT;

        const bool okColor = SUCCEEDED(m_device->CheckMultisampleQualityLevels(colorFmt, newSamples, &qColor)) && (qColor > 0);
        const bool okDepth = SUCCEEDED(m_device->CheckMultisampleQualityLevels(depthFmt, newSamples, &qDepth)) && (qDepth > 0);

        if (!okColor || !okDepth)
        {
            // fallback
            newSamples = 1;
            newQuality = 0;
            std::cout << "[MSAA] Requested not supported. Fallback to 1.\n";
        }
        else
        {
            const UINT qCommon = (qColor < qDepth) ? qColor : qDepth;
            newQuality = qCommon - 1;
        }
    }

    // 기존 MSAA 리소스 해제
    m_msaaRT = {};
    m_msaaDepthTex.Reset();
    m_msaaDSV.Reset();

    // 값 반영
    m_msaaSamples = newSamples;
    m_msaaQuality = newQuality;

    // MSAA 리소스 재생성 (크기 의존)
    if (!CreateMSAARenderTarget(m_width, m_height))
        return false;

    if (!CreateMSAADepthStencil(m_width, m_height))
        return false;

    //std::cout << "[MSAA] Applied Samples=" << m_msaaSamples << " Quality=" << m_msaaQuality << "\n";
    return true;
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
// Shader
// ------------------------------------------------------------

// 셰이더 로드 함수임 filePath 같은 경우 "../Assets/Shaders/~~.hlsl" or L"Assets/Shaders/~~.hlsl" 2가지 방법 전부 가능
bool YunoRenderer::LoadShader(
    ShaderId id,
    const std::wstring& filePath,
    const std::string& vsEntry,
    const std::string& psEntry,
    const std::string& vsProfile,
    const std::string& psProfile,
    const std::vector<std::pair<std::string, std::string>>& defines)
{
    if (!m_device)
        return false;

    YunoShaderCompiler compiler;

    {
        std::filesystem::path p(filePath);
        compiler.AddIncludeDir(p.parent_path().wstring());
    }

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

    try
    {
        vsBlob = compiler.CompileFromFile(filePath, vsEntry, vsProfile, defines);
        psBlob = compiler.CompileFromFile(filePath, psEntry, psProfile, defines);
    }
    catch (const std::exception& e)
    {
        std::cout << "[LoadShader] Shader compile failed: " << std::string(vsEntry) << "/" << std::string(psEntry) << "\n";
        std::cout << e.what() << "\n";

        return false;
    }

    if (!vsBlob || !psBlob)
        return false;

    ShaderProgram prog{};
    prog.vsBytecode = vsBlob;

    prog.vs = std::make_unique<YunoShader>();
    prog.ps = std::make_unique<YunoShader>();

    if (!prog.vs->CreateVertexShader(m_device.Get(), vsBlob.Get()))
        return false;
    if (!prog.ps->CreatePixelShader(m_device.Get(), psBlob.Get()))
        return false;
    

    m_programs[id] = std::move(prog);
    return true;
}

static std::wstring Utf8ToWString(const char* s)
{
    if (!s) return {};
    const int len = static_cast<int>(std::strlen(s));
    if (len == 0) return {};

    const int wlen = MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
    if (wlen <= 0) return {};

    std::wstring w;
    w.resize(wlen);
    MultiByteToWideChar(CP_UTF8, 0, s, len, w.data(), wlen);
    return w;
}

bool YunoRenderer::LoadShader(
    ShaderId id,
    const char* filePath,
    const char* vsEntry,
    const char* psEntry,
    const char* vsProfile,
    const char* psProfile)
{
    if (!filePath || !vsEntry || !psEntry) return false;

    const std::wstring wPath = Utf8ToWString(filePath);
    const std::string  sVsEntry = vsEntry;
    const std::string  sPsEntry = psEntry;
    const std::string  sVsProfile = vsProfile ? vsProfile : "vs_5_0";
    const std::string  sPsProfile = psProfile ? psProfile : "ps_5_0";

    return LoadShader(id, wPath, sVsEntry, sPsEntry, sVsProfile, sPsProfile);
}


// desc 체크해서 셰이더 지정해주는 함수
const ShaderId YunoRenderer::SetShaderKey(const MaterialDesc& desc)
{
    if (desc.passKey.vertexFlags & (VSF_BoneIndex | VSF_BoneWeight))
    {
        if (desc.orm != 0)
            return ShaderId::PBRAnimation;

        if (desc.metal != 0 || desc.rough != 0 || desc.ao != 0)
            return ShaderId::PBRAnimation;

        return ShaderId::BasicAnimation;
    }
    if (desc.orm != 0)
        return ShaderId::PBRBase;

    if (desc.metal != 0 || desc.rough != 0 || desc.ao != 0)
        return ShaderId::PBRBase;

    return ShaderId::Basic;
}


// ------------------------------------------------------------
// Texture & Sampler
// ------------------------------------------------------------

// 단색 텍스쳐 만들어내는 함수
TextureHandle YunoRenderer::CreateTexture2D_SolidRGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (!m_device) return 0;

    const uint32_t pixel = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);

    D3D11_TEXTURE2D_DESC td{};
    td.Width = 1;
    td.Height = 1;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = &pixel;
    init.SysMemPitch = 4;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    if (FAILED(m_device->CreateTexture2D(&td, &init, tex.GetAddressOf())))
        return 0;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    if (FAILED(m_device->CreateShaderResourceView(tex.Get(), nullptr, srv.GetAddressOf())))
        return 0;

    TextureResource tr{};
    tr.srv = srv;
    tr.w = 1;
    tr.h = 1;

    m_textures.push_back(std::move(tr));
    return static_cast<TextureHandle>(m_textures.size()); // 1-based
}

bool YunoRenderer::CreateDefaultTextures()
{
    // Albedo default = white
    m_texWhite = CreateTexture2D_SolidRGBA8(255, 255, 255, 255);
    if (m_texWhite == 0) return false;

    // Normal default = (0.5, 0.5, 1.0) -> (128,128,255)
    m_texNormal = CreateTexture2D_SolidRGBA8(128, 128, 255, 255);
    if (m_texNormal == 0) return false;

    // ORM default: 일단 black (필요하면 (255,255,255)로 바꿔도 됨)
    m_texBlack = CreateTexture2D_SolidRGBA8(0, 0, 0, 255);
    if (m_texBlack == 0) return false;

    return true;
}

ID3D11ShaderResourceView* YunoRenderer::ResolveSRV(TextureHandle h) const
{
    if (h == 0 || h > m_textures.size())
        return nullptr;
    return m_textures[h - 1].srv.Get();
}



bool YunoRenderer::CreateSamplerPresets()
{
    if (!m_device)
        return false;

    auto MakeAniso = [&](D3D11_TEXTURE_ADDRESS_MODE addr, const float border[4])
        -> Microsoft::WRL::ComPtr<ID3D11SamplerState>
        {
            D3D11_SAMPLER_DESC sd{};
            sd.Filter = D3D11_FILTER_ANISOTROPIC;
            sd.AddressU = addr;
            sd.AddressV = addr;
            sd.AddressW = addr;

            sd.MipLODBias = 0.0f;
            sd.MaxAnisotropy = 16; // 보통 16 고정(필요하면 옵션화)
            sd.ComparisonFunc = D3D11_COMPARISON_NEVER;

            sd.BorderColor[0] = border[0];
            sd.BorderColor[1] = border[1];
            sd.BorderColor[2] = border[2];
            sd.BorderColor[3] = border[3];

            sd.MinLOD = 0.0f;
            sd.MaxLOD = D3D11_FLOAT32_MAX;

            Microsoft::WRL::ComPtr<ID3D11SamplerState> samp;
            if (FAILED(m_device->CreateSamplerState(&sd, samp.GetAddressOf())))
                return {};

            return samp;
        };


    {
        const float border[4] = { 0,0,0,0 };
        m_samplers[(uint8_t)SampleMode::Custom] = MakeAniso(D3D11_TEXTURE_ADDRESS_WRAP, border);
        if (!m_samplers[(uint8_t)SampleMode::Custom]) return false;
    }

    // 1) Wrap + Aniso
    {
        const float border[4] = { 0,0,0,0 };
        m_samplers[(uint8_t)SampleMode::WrapAniso] = MakeAniso(D3D11_TEXTURE_ADDRESS_WRAP, border);
        if (!m_samplers[(uint8_t)SampleMode::WrapAniso]) return false;
    }

    // 2) Clamp + Aniso
    {
        const float border[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_samplers[(uint8_t)SampleMode::ClampAniso] = MakeAniso(D3D11_TEXTURE_ADDRESS_CLAMP, border);
        if (!m_samplers[(uint8_t)SampleMode::ClampAniso]) return false;
    }

    // 3) Border(0,0,0,0) + Aniso
    {
        const float border[4] = { 0,0,0,0 };
        m_samplers[(uint8_t)SampleMode::Border0000Aniso] = MakeAniso(D3D11_TEXTURE_ADDRESS_BORDER, border);
        if (!m_samplers[(uint8_t)SampleMode::Border0000Aniso]) return false;
    }

    // 4) Border(0,0,0,0) + MIP_POINT (섀도우맵 전용)
    {
        D3D11_SAMPLER_DESC sd{};
        sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

        sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

        sd.BorderColor[0] = 1.0f;
        sd.BorderColor[1] = 1.0f;
        sd.BorderColor[2] = 1.0f;
        sd.BorderColor[3] = 1.0f;

        sd.MinLOD = 0;
        sd.MaxLOD = D3D11_FLOAT32_MAX;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> samp;
        if (FAILED(m_device->CreateSamplerState(&sd, samp.GetAddressOf())))
            return {};

        m_samplers[(uint8_t)SampleMode::ShadowMap] = samp;
        if (!m_samplers[(uint8_t)SampleMode::ShadowMap]) return false;
    }

    return true;
}

// ------------------------------------------------------------
// Game -> Engine API 구현
// ------------------------------------------------------------

MeshHandle YunoRenderer::CreateMesh(const VertexStreams& streams,
    const INDEX* triangles,
    uint32_t triCount)
{
    auto Has = [&](uint32_t f) { return (streams.flags & f) != 0; };

    const uint32_t indexCount = triCount * 3;
    const uint32_t* indices = (triCount > 0)
        ? reinterpret_cast<const uint32_t*>(triangles)
        : nullptr;

    if (!m_device)
        return 0;

    // Position은 필수로 있어야 됨
    //assert(Has(VSF_Pos) && streams.pos != nullptr && streams.vtx_count != 0);
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
    if (Has(VSF_BoneWeight) && streams.boneWeight == nullptr) return 0;
    if (Has(VSF_BoneIndex) && streams.boneIdx == nullptr) return 0;

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

    switch (streams.topology) 
    {
    case Yuno_LINELIST:
        mr.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case Yuno_LINESTRIP:
        mr.topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case Yuno_TRIANGLESTRIP:
        mr.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    default:
        mr.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    }

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

    if (Has(VSF_BoneWeight))
    {
        if (!CreateVB(streams.boneWeight, sizeof(VERTEX_BoneWeight), streams.vtx_count, mr.vbBoneWeight))
            return 0;
    }

    if (Has(VSF_BoneIndex))
    {
        if (!CreateVB(streams.boneIdx, sizeof(VERTEX_BoneIndex), streams.vtx_count, mr.vbBoneIndex))
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

MaterialHandle YunoRenderer::CreateMaterial(const MaterialDesc& desc)
{
    PassKey key = desc.passKey;

    if (key.vs == ShaderId::Basic) // 이게 베이직이 아니라는 것은 외부에서 지정했다는 거니까 통과 ㄱㄱ
    {
        const ShaderId id = SetShaderKey(desc);
        key.vs = id;
        key.ps = id;

        // MaterialDesc 정보를 확인해서 셰이더뭐쓸지 결정하는곳
    }

    const RenderPassHandle pass = GetOrCreatePass(key);
    if (pass == 0)
        return 0;

    YunoMaterial mat{};
    mat.pass = pass;

    mat.isAlphaBlend = desc.passKey.blend != BlendPreset::Opaque;
    mat.albedo = desc.albedo;
    mat.normal = desc.normal;
    mat.orm = desc.orm;
    mat.emissive = desc.emissive;
    mat.opacity = desc.opacity;
    mat.metallic = desc.metal;
    mat.roughness = desc.rough;
    mat.ao = desc.ao;

    m_materials.push_back(mat);
    return static_cast<MaterialHandle>(m_materials.size()); // 1-based
}


MaterialHandle YunoRenderer::CreateMaterial_Default()
{
    if (m_defaultMaterial != 0)
        return m_defaultMaterial;

    MaterialDesc md{};
    md.passKey.vs = ShaderId::Basic;
    md.passKey.ps = ShaderId::Basic;
    md.passKey.vertexFlags = VSF_Pos | VSF_Nrm | VSF_UV;
    md.passKey.blend = BlendPreset::Opaque;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadWrite;


    const MaterialHandle h = CreateMaterial(md);
    if (h == 0) return 0;

    m_defaultMaterial = h;
    return m_defaultMaterial;
}


TextureHandle YunoRenderer::CreateColorTexture2DFromFile(const wchar_t* path)
{
    if (!m_device || !path || !path[0])
        return 0;

    // 1) 경로 정규화
    std::filesystem::path p(path);

    std::error_code ec;
    std::filesystem::path abs = std::filesystem::absolute(p, ec);
    if (!ec)
        abs = std::filesystem::weakly_canonical(abs, ec);

    // 캐시 키
    const std::wstring key = (!ec) ? abs.wstring() : std::wstring(path);

    // 2) 캐시 hit
    auto it = m_texturePathCache.find(key);
    if (it != m_texturePathCache.end())
        return it->second;

    // 3) WIC 로딩 (RGBA32F)
    ImageRGBA8 img{};
    if (!LoadImageRGBA8_WIC(key.c_str(), img))
    {
        return 0;
    }

    // 4) D3D11 Texture2D 생성
    D3D11_TEXTURE2D_DESC td{};
    td.Width = img.width;
    td.Height = img.height;
    td.MipLevels = 1;                 
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = img.pixels.data();
    init.SysMemPitch = img.width * 4;
    init.SysMemSlicePitch = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    HRESULT hr = m_device->CreateTexture2D(&td, &init, tex.GetAddressOf());
    if (FAILED(hr))
        return 0;

    // 5) SRV 생성
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = m_device->CreateShaderResourceView(tex.Get(), nullptr, srv.GetAddressOf());
    if (FAILED(hr))
        return 0;

    // 6) 테이블에 저장하고 handle 발급
    TextureResource tr{};
    tr.srv = srv;
    tr.w = img.width;
    tr.h = img.height;

    m_textures.push_back(std::move(tr));
    const TextureHandle handle = static_cast<TextureHandle>(m_textures.size()); // 1-based

    // 7) path 캐시 등록
    m_texturePathCache.emplace(key, handle);

    return handle;
}

TextureHandle YunoRenderer::CreateDataTexture2DFromFile(const wchar_t* path)
{
    if (!m_device || !path || !path[0])
        return 0;

    // 1) 경로 정규화
    std::filesystem::path p(path);

    std::error_code ec;
    std::filesystem::path abs = std::filesystem::absolute(p, ec);
    if (!ec)
        abs = std::filesystem::weakly_canonical(abs, ec);

    // 캐시 키
    const std::wstring key = (!ec) ? abs.wstring() : std::wstring(path);

    // 2) 캐시 hit
    auto it = m_texturePathCache.find(key);
    if (it != m_texturePathCache.end())
        return it->second;

    // 3) WIC 로딩 (RGBA32F)
    ImageRGBA8 img{};
    if (!LoadImageRGBA8_WIC(key.c_str(), img))
    {
        return 0;
    }

    // 4) D3D11 Texture2D 생성
    D3D11_TEXTURE2D_DESC td{};
    td.Width = img.width;
    td.Height = img.height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = img.pixels.data();
    init.SysMemPitch = img.width * 4;
    init.SysMemSlicePitch = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    HRESULT hr = m_device->CreateTexture2D(&td, &init, tex.GetAddressOf());
    if (FAILED(hr))
        return 0;

    // 5) SRV 생성
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = m_device->CreateShaderResourceView(tex.Get(), nullptr, srv.GetAddressOf());
    if (FAILED(hr))
        return 0;

    // 6) 테이블에 저장하고 handle 발급
    TextureResource tr{};
    tr.srv = srv;
    tr.w = img.width;
    tr.h = img.height;

    m_textures.push_back(std::move(tr));
    const TextureHandle handle = static_cast<TextureHandle>(m_textures.size()); // 1-based

    // 7) path 캐시 등록
    m_texturePathCache.emplace(key, handle);

    return handle;
}

std::pair<int, int> YunoRenderer::GetTextureSize(TextureHandle handle) const
{
    if (handle == 0 || handle > m_textures.size())
        return { -1, -1 };
    const TextureResource& tr = m_textures[handle - 1];
    return { static_cast<int>(tr.w), static_cast<int>(tr.h) };

}

void YunoRenderer::SetPostProcessOption(uint32_t flag)
{
    if(m_PPFlag = 0)
        m_PPFlag = PostProcessFlag::Default;
    m_PPFlag |= flag;

    SetPP_Pass();
}

void YunoRenderer::ResetPostProcessOption()
{
    m_PPFlag = PostProcessFlag::Default;

    SetPP_Pass();
}

RenderPassHandle YunoRenderer::GetOrCreatePass(const PassKey& key)
{
    auto it = m_passCache.find(key);
    if (it != m_passCache.end())
        return it->second; // 패스가 이미 있으면 그거 주고 없으면 새로만듬

    if (key.vs != key.ps && key.ps != ShaderId::None)
        return 0;

    auto itProg = m_programs.find(key.vs);
    if (itProg == m_programs.end())
        return 0;

    const ShaderProgram& shader = itProg->second;
    if (!shader.vs || !shader.ps || !shader.vsBytecode)
        return 0;

    YunoRenderPassDesc pd{};
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

    if (key.domain == MaterialDomain::Surface)
    {
        if (!InputLayoutFromFlags(key.vertexFlags, layout)) // 플래그에 맞는 layout을 만들어서 담아줌
            return 0;

        pd.inputElements = layout.data();                                   // 키 기반 완료
        pd.inputElementCount = static_cast<uint32_t>(layout.size());        // 키 기반 완료
    }
    
    pd.vs = shader.vs.get();                                            // 키 기반 완료
    if (key.ps != ShaderId::None)
        pd.ps = shader.ps.get();                                            // 키 기반 완료
    else
    {
        pd.ps = m_programs[ShaderId::None].ps.get();
    }
    pd.vsBytecode = shader.vsBytecode.Get();                            // 키 기반 완료
    pd.domain = key.domain;
    

    switch (key.depth)                                                  // 키 기반 완료
    {
    case DepthPreset::Off:       pd.depth = DepthMode::Off;       break;
    case DepthPreset::ReadOnly:  pd.depth = DepthMode::ReadOnly;  break;
    case DepthPreset::ReadWrite: pd.depth = DepthMode::ReadWrite; break;
    default:                     pd.depth = DepthMode::ReadWrite; break;
    }


    // 블렌드 모드
    switch (key.blend)                                                  // 키 기반 완료
    {
    case BlendPreset::Opaque:        pd.blend = BlendMode::Opaque; break;
    case BlendPreset::AlphaBlend:    pd.blend = BlendMode::AlphaBlend; break;
    case BlendPreset::ColorBlend:    pd.blend = BlendMode::ColorBlend; break;
    case BlendPreset::ColorBlendOne: pd.blend = BlendMode::ColorBlendOne; break;
    default:                         pd.blend = BlendMode::Opaque; break;
    } 
    

    pd.wireframe =(key.raster == RasterPreset::WireCullNone)            // 키 기반 완료
                ||(key.raster == RasterPreset::WireCullBack) 
                ||(key.raster == RasterPreset::WireCullFront);


    // 컬 모드
    switch (key.raster)                                                 // 키 기반 완료
    {
    case RasterPreset::CullNone:      pd.cull = CullMode::None;  break;
    case RasterPreset::CullBack:      pd.cull = CullMode::Back;  break;
    case RasterPreset::CullFront:     pd.cull = CullMode::Front; break;

    case RasterPreset::WireCullNone:  pd.cull = CullMode::None;  break;
    case RasterPreset::WireCullBack:  pd.cull = CullMode::Back;  break;
    case RasterPreset::WireCullFront: pd.cull = CullMode::Front; break;
    default:                          pd.cull = CullMode::None;  break;
    }

    // RenderpassDesk 생성한걸로 렌더패스 생성

    auto pass = std::make_unique<YunoRenderPass>();
    if (!pass->Create(m_device.Get(), pd))
        return 0;

    m_passes.push_back(std::move(pass));
    const RenderPassHandle handle = static_cast<RenderPassHandle>(m_passes.size());

    m_passCache.emplace(key, handle);
    return handle;
}

bool YunoRenderer::InputLayoutFromFlags(uint32_t flags,
    std::vector<D3D11_INPUT_ELEMENT_DESC>& outLayout) const
{
    outLayout.clear();

    // 필수: POSITION
    if ((flags & VSF_Pos) == 0)
        return false;

    // 슬롯 규약
    // 0=Pos, 1=Nrm, 2=UV, 3=T, 4=B, 5=BoneWeight, 6=BoneIndex
    outLayout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0 });

    if (flags & VSF_Nrm)
    {
        outLayout.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }

    if (flags & VSF_UV)
    {
        outLayout.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }

    if (flags & VSF_T)
    {
        outLayout.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }

    if (flags & VSF_B)
    {
        outLayout.push_back({ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }

    if (flags & VSF_BoneWeight)
    {
        outLayout.push_back({ "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }

    if (flags & VSF_BoneIndex)
    {
        outLayout.push_back({ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 6, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0 });
    }

    return true;
}



// ------------------------------------------------------------
// Render
// ------------------------------------------------------------

void YunoRenderer::Submit(const RenderItem& item)
{
    if (item.meshHandle == 0 || item.meshHandle > m_meshes.size())
        return;

    RenderItem copy = item;
    if (copy.materialHandle == 0)
        copy.materialHandle = m_defaultMaterial;

    if (copy.materialHandle == 0 || copy.materialHandle > m_materials.size())
        return;

    if (m_materials[item.materialHandle - 1].isAlphaBlend)
    {
        m_renderBlendQueue.push_back(copy);
    }
    else
        m_renderQueue.push_back(copy);
}

void YunoRenderer::Flush()
{
    if (!m_context || !m_cbFrame.IsValid() || !m_cbObject_Matrix.IsValid() || !m_cbObject_Material.IsValid() || !m_cbLight.IsValid())
        return;

    // 렌더 전에 정렬 넣을예정
   /* XMVECTOR campos = XMLoadFloat3(&m_camera.Position());
    XMVECTOR camForward = m_camera.GetForward();*/
    XMMATRIX view = m_camera.View();

    for (auto& item : m_renderBlendQueue)
    {
        XMVECTOR pos = XMLoadFloat3(&item.Constant.worldPos);
        XMVECTOR viewPos = XMVector3TransformCoord(pos, view);
        item.sortkey = XMVectorGetZ(viewPos);
    }

    std::sort(m_renderBlendQueue.begin(), m_renderBlendQueue.end(),
        [](const RenderItem& r, const RenderItem& l)
        {
            return r.sortkey > l.sortkey;
        });

#ifdef _DEBUG
    SubmitDebugPointLightMesh();
#endif

    // 샘플러 바인드
    BindSamplers();

    for (const RenderItem& item : m_renderQueue)
    {
        if (item.meshHandle == 0 || item.meshHandle > m_meshes.size())
            continue;

        const YunoMaterial* material = nullptr;

        if (item.materialHandle > 0 && item.materialHandle <= m_materials.size()) // 핸들 유효성 체크
            material = &m_materials[item.materialHandle - 1];

        if (!material)
        {
            if (m_defaultMaterial > 0 && m_defaultMaterial <= m_materials.size())
                material = &m_materials[m_defaultMaterial - 1];
        }


        RenderPassHandle passHandle = material->pass;

        if (passHandle == 0)
            passHandle = m_defaultPass;

        if (passHandle == 0 || passHandle > m_passes.size())
            continue;

        // 렌더 패스 바인드
        m_passes[passHandle - 1]->Bind(m_context.Get());

        // 메쉬 바인드
        const MeshResource& mesh = m_meshes[item.meshHandle - 1];
        mesh.Bind(m_context.Get());

        // 상수 버퍼 바인드
        BindConstantBuffers(item);

        // 텍스쳐 바인드
        BindTextures(*material);

        // 드로우
        if (mesh.ib && mesh.indexCount > 0)
            m_context->DrawIndexed(mesh.indexCount, 0, 0);
        else
            m_context->Draw(mesh.vertexCount, 0);
    }

    m_renderQueue.clear();

    for (auto& item : m_renderBlendQueue)
    {
        if (item.meshHandle == 0 || item.meshHandle > m_meshes.size())
            continue;

        const YunoMaterial* material = nullptr;

        if (item.materialHandle > 0 && item.materialHandle <= m_materials.size()) // 핸들 유효성 체크
            material = &m_materials[item.materialHandle - 1];

        if (!material)
        {
            if (m_defaultMaterial > 0 && m_defaultMaterial <= m_materials.size())
                material = &m_materials[m_defaultMaterial - 1];
        }

        RenderPassHandle passHandle = material->pass;

        if (passHandle == 0)
            passHandle = m_defaultPass;

        if (passHandle == 0 || passHandle > m_passes.size())
            continue;

        // 렌더 패스 바인드
        m_passes[passHandle - 1]->Bind(m_context.Get());

        // 메쉬 바인드
        const MeshResource& mesh = m_meshes[item.meshHandle - 1];
        mesh.Bind(m_context.Get());

        // 상수 버퍼 바인드
        BindConstantBuffers(item);

        // 텍스쳐 바인드
        BindTextures(*material);

        // 드로우
        if (mesh.ib && mesh.indexCount > 0)
            m_context->DrawIndexed(mesh.indexCount, 0, 0);
        else
            m_context->Draw(mesh.vertexCount, 0);
    }

    m_renderBlendQueue.clear();
}

// ------------------------------------------------------------
// Bind 함수들
// ------------------------------------------------------------

void YunoRenderer::BindConstantBuffers(const RenderItem& item)
{
    using namespace DirectX;


    // -----------------------------
    // CBPerObject_Matrix (b0)
    // -----------------------------
    CBPerObject_Matrix cbPerObject_matrix{};

    XMMATRIX V = m_camera.View();
    XMMATRIX P = m_camera.Proj(
        YunoEngine::GetWindow()->GetClientWidth(),
        YunoEngine::GetWindow()->GetClientHeight()
    );
    XMMATRIX W = XMLoadFloat4x4(&item.Constant.world);
    XMMATRIX WVP = W * V * P;

    XMMATRIX mAnim[MAX_BONE];
    if (item.haveAnim)
    {
        size_t i = 0;
        for (auto& animTM : item.Constant.boneAnim)
        {
            mAnim[i] = XMLoadFloat4x4(&animTM);
            i++;
        }
    }
    
    XMStoreFloat4x4(&cbPerObject_matrix.mWorld, XMMatrixTranspose(W));
    XMStoreFloat4x4(&cbPerObject_matrix.mWVP, XMMatrixTranspose(WVP));
    XMStoreFloat4x4(&cbPerObject_matrix.mWInvT, (XMMatrixInverse(nullptr, W)));
    
    if (item.haveAnim)
    {
        size_t i = 0;
        for (auto& animTM : mAnim)
        {
            XMStoreFloat4x4(&cbPerObject_matrix.mBoneAnim[i], XMMatrixTranspose(animTM));
            i++;
        }
    }

    m_cbObject_Matrix.Update(m_context.Get(), cbPerObject_matrix);

    ID3D11Buffer* cbPerObj_Matrix_Buffers[] = { m_cbObject_Matrix.Get() };
    m_context->VSSetConstantBuffers(0, 1, cbPerObj_Matrix_Buffers);
    m_context->PSSetConstantBuffers(0, 1, cbPerObj_Matrix_Buffers);


    // -----------------------------
    // CBPerObject_Matetrial (b1)
    // -----------------------------
    CBPerObject_Material cbPerObject_material{};

    cbPerObject_material.baseColor = item.Constant.baseColor;
    cbPerObject_material.emissiveColor = item.Constant.emissiveColor;
    cbPerObject_material.roughRatio = item.Constant.roughRatio;
    cbPerObject_material.metalRatio = item.Constant.metalRatio;
    cbPerObject_material.shadowBias = item.Constant.shadowBias;
    cbPerObject_material.opacity = item.Constant.opacity;

    m_cbObject_Material.Update(m_context.Get(), cbPerObject_material);

    ID3D11Buffer* cbPerObj_Material_Buffers[] = { m_cbObject_Material.Get() };
    m_context->VSSetConstantBuffers(1, 1, cbPerObj_Material_Buffers);
    m_context->PSSetConstantBuffers(1, 1, cbPerObj_Material_Buffers);
}


void YunoRenderer::BindConstantBuffers_Camera(const Frame_Data_Dir& dirData)
{
    using namespace DirectX;
    // -----------------------------
    // CBPerFrame (b2)
    // -----------------------------
    CBPerFrame cbPerFrame{};

    

    XMMATRIX V = m_camera.View();
    XMMATRIX P = m_camera.Proj(
        YunoEngine::GetWindow()->GetClientWidth(), 
        YunoEngine::GetWindow()->GetClientHeight()
    );


    XMStoreFloat4x4(&cbPerFrame.mView, XMMatrixTranspose(V));
    XMStoreFloat4x4(&cbPerFrame.mProj, XMMatrixTranspose(P));
    cbPerFrame.camPos = m_camera.Position();

    m_cbFrame.Update(m_context.Get(), cbPerFrame);

    ID3D11Buffer* cbFrame[] = { m_cbFrame.Get() };
    m_context->VSSetConstantBuffers(2, 1, cbFrame);
    m_context->PSSetConstantBuffers(2, 1, cbFrame);

}

void YunoRenderer::BindConstantBuffers_Light(const Frame_Data_Dir& dirData, const std::vector<Frame_Data_Point>& plData, UINT plCount)
{
    using namespace DirectX;
    // -----------------------------
    // CBLight (b3)
    // -----------------------------
    m_LightInfo.dirLit.dir = dirData.Lightdir;
    m_LightInfo.dirLit.diff = dirData.Lightdiff;
    m_LightInfo.dirLit.amb = dirData.Lightamb;
    m_LightInfo.dirLit.spec = dirData.Lightspec;
    m_LightInfo.dirLit.intensity.x = dirData.intensity;

    if (plCount)
    {
        int i = 0;
        for (auto& data : plData)
        {
            m_LightInfo.pointLit[i].col = data.col;
            m_LightInfo.pointLit[i].pos_intensity.x = data.pos.x;
            m_LightInfo.pointLit[i].pos_intensity.y = data.pos.y;
            m_LightInfo.pointLit[i].pos_intensity.z = data.pos.z;
            m_LightInfo.pointLit[i].pos_intensity.w = data.intensity;
            
            #ifdef _DEBUG
            if(i < plCount)
                debuglights.push_back({ data.pos, data.col });
            #endif

            i++;
        }
        m_LightInfo.plCount.x = plCount;
    }

    m_cbLight.Update(m_context.Get(), m_LightInfo);
    m_cbShadow.Update(m_context.Get(), m_shadowInfo);

    ID3D11Buffer* cbLight[] = { m_cbLight.Get() };
    m_context->VSSetConstantBuffers(3, 1, cbLight);
    m_context->PSSetConstantBuffers(3, 1, cbLight);

    ID3D11Buffer* cbShadow = m_cbShadow.Get();
    m_context->VSSetConstantBuffers(4, 1, &cbShadow);
    m_context->PSSetConstantBuffers(4, 1, &cbShadow);
}


void YunoRenderer::BindSamplers()
{
    ID3D11SamplerState* samps[(uint8_t)SampleMode::Count] =
    {
        m_samplers[(uint8_t)SampleMode::Custom].Get(),
        m_samplers[(uint8_t)SampleMode::WrapAniso].Get(),
        m_samplers[(uint8_t)SampleMode::ClampAniso].Get(),
        m_samplers[(uint8_t)SampleMode::Border0000Aniso].Get(),
        m_samplers[(uint8_t)SampleMode::ShadowMap].Get(),
    };

    m_context->PSSetSamplers(0, (uint8_t)SampleMode::Count, samps);
}


void YunoRenderer::BindTextures(const YunoMaterial& material)
{
    TextureHandle hAlbedo = (material.albedo != 0) ? material.albedo : m_texWhite;
    TextureHandle hNormal = (material.normal != 0) ? material.normal : m_texNormal;

    TextureHandle hOrm = (material.orm != 0) ? material.orm : m_texBlack;
    TextureHandle hEmissive = (material.emissive != 0) ? material.emissive : m_texBlack;
    TextureHandle hOpacity = (material.opacity != 0) ? material.opacity : m_texWhite;

    TextureHandle hMetallic = (material.metallic != 0) ? material.metallic : m_texBlack;
    TextureHandle hRoughness = (material.roughness != 0) ? material.roughness : m_texBlack;
    TextureHandle hAO = (material.ao != 0) ? material.ao : m_texBlack;

    ID3D11ShaderResourceView* srvs[9] =
    {
        ResolveSRV(hAlbedo),
        ResolveSRV(hNormal),
        ResolveSRV(hOrm),
        ResolveSRV(hMetallic),
        ResolveSRV(hRoughness),
        ResolveSRV(hAO),
        ResolveSRV(hEmissive),
        ResolveSRV(hOpacity),
        m_ShadowMap.dssrv.Get()
    };

    // 혹시라도 ResolveSRV가 nullptr이면 안전하게 nullptr 바인딩(디버그에서 보이게)
    m_context->PSSetShaderResources(0, 9, srvs);
}

// ------------------------------------------------------------
// Debug 함수들
// ------------------------------------------------------------
#ifdef _DEBUG
void YunoRenderer::CreateDebugMeshResources()
{
    if (m_debugSphereMeshHandle != 0 && m_debugMeshMaterial != 0)
        return;

    float radius = 1.0f;
    int sliceCount = 8;
    int stackCount = 8;

    std::vector<VERTEX_Pos> v;
    std::vector<INDEX> idx;
    //vertex
    v.push_back({ 0, radius, 0 });

    for (int stack = 1; stack <= stackCount - 1; ++stack)
    {
        float phi = XM_PI * stack / stackCount;

        for (int slice = 0; slice <= sliceCount; ++slice)
        {
            float theta = XM_2PI * slice / sliceCount;

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            v.push_back({ x, y, z});
        }
    }
    v.push_back({ 0, -radius, 0 });

    //index
    for (uint32_t i = 1; i <= sliceCount; ++i)
    {
        idx.push_back({0, i, i + 1});
    }

    int baseIndex = 1;
    int ringVertexCount = sliceCount + 1;

    for (int stack = 0; stack < stackCount - 2; ++stack)
    {
        for (int slice = 0; slice < sliceCount; ++slice)
        {
            uint32_t i0 = baseIndex + stack * ringVertexCount + slice;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = i0 + ringVertexCount;
            uint32_t i3 = i2 + 1;

            // Triangle 1
            idx.push_back({ i0, i2, i2 });

            // Triangle 2
            idx.push_back({ i1, i2, i3 });
        }
    }

    uint32_t southPoleIndex = (uint32_t)v.size() - 1;
    uint32_t lastRingStart = southPoleIndex - ringVertexCount;

    for (int i = 0; i < sliceCount; ++i)
    {
        idx.push_back({ southPoleIndex, lastRingStart + i + 1, lastRingStart + i });
    }

    VertexStreams vs;
    vs.flags = VSF_Pos;
    vs.vtx_count = v.size() * 3;
    vs.pos = v.data();

    m_debugSphereMeshHandle = CreateMesh(vs, idx.data(), idx.size());
    if (!m_debugSphereMeshHandle) return;


    MaterialDesc md{};
    md.passKey.vs = ShaderId::DebugMesh;
    md.passKey.ps = ShaderId::DebugMesh;
    md.passKey.vertexFlags = VSF_Pos;
    md.passKey.blend = BlendPreset::Opaque;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadOnly;

    const MaterialHandle h = CreateMaterial(md);
    if (h == 0) return;

    m_debugMeshMaterial = h;
}

void YunoRenderer::CreateDebugGridResources()
{
    if (m_debugGridMeshHandle != 0 && m_debugGridMaterial != 0)
        return;

    constexpr int   N = 12;
    constexpr float s = 5.0f;
    constexpr float half = N * s;
    constexpr float y = 0.01f;

    std::vector<VERTEX_Pos> v;
    v.reserve((N * 2 + 1) * 4); 

    for (int i = -N; i <= N; ++i)
    {
        const float z = i * s;
        v.push_back(VERTEX_Pos{ -half, y, z });
        v.push_back(VERTEX_Pos{ +half, y, z });

        const float x = i * s;
        v.push_back(VERTEX_Pos{ x, y, -half });
        v.push_back(VERTEX_Pos{ x, y, +half });
    }

    m_debugGridMeshHandle = CreateLineMesh_PosOnly(v.data(), (uint32_t)v.size());
    if (m_debugGridMeshHandle == 0)
        return;

    MaterialDesc md{};
    md.passKey.vs = ShaderId::DebugGrid;
    md.passKey.ps = ShaderId::DebugGrid;
    md.passKey.vertexFlags = VSF_Pos;

    md.passKey.blend = BlendPreset::Opaque;
    md.passKey.raster = RasterPreset::CullNone;
    md.passKey.depth = DepthPreset::ReadWrite;

    //md.baseColor = { 1,1,0,1 };
    //md.roughRatio = 1.0f;
    //md.metalRatio = 1.0f;

    m_debugGridMaterial = CreateMaterial(md);
}

void YunoRenderer::SubmitDebugGrid()
{
    if (!m_enableDebugGrid)
        return;

    if (m_debugGridMeshHandle == 0)
        return;

    RenderItem item{};
    item.meshHandle = m_debugGridMeshHandle;
    item.materialHandle = m_debugGridMaterial; // 0이어도 Submit에서 default로 보정됨
    DirectX::XMStoreFloat4x4(&item.Constant.world, DirectX::XMMatrixIdentity());
    item.Constant.baseColor = { 1, 1, 0, 1 };
    item.Constant.roughRatio = 1.0f;
    item.Constant.metalRatio = 1.0f;
    item.Constant.shadowBias = 0.005f;

    Submit(item);
}

void YunoRenderer::SubmitDebugPointLightMesh()
{
    if (m_debugSphereMeshHandle == 0)
        return;

    float scale = 0.05f;

    for (auto& pl : debuglights)
    {
        RenderItem item{};
        item.meshHandle = m_debugSphereMeshHandle;
        item.materialHandle = m_debugMeshMaterial;
        DirectX::XMStoreFloat4x4(&item.Constant.world, XMMatrixScaling(scale, scale, scale) * XMMatrixTranslation(pl.pos.x, pl.pos.y, pl.pos.z));
        item.Constant.baseColor = pl.col;

        Submit(item);
    }

    debuglights.clear();
}


void YunoRenderer::DrawDebug()
{
    m_renderQueue.clear();

    //디버그 관련 그릴것들 서브밋
    SubmitDebugGrid();

    if (!m_context || !m_cbFrame.IsValid() || !m_cbObject_Matrix.IsValid() || !m_cbObject_Material.IsValid() || !m_cbLight.IsValid())
        return;

    // 샘플러 바인드
    BindSamplers();

    for (const RenderItem& item : m_renderQueue)
    {
        if (item.meshHandle == 0 || item.meshHandle > m_meshes.size())
            continue;

        const YunoMaterial* material = nullptr;

        if (item.materialHandle > 0 && item.materialHandle <= m_materials.size()) // 핸들 유효성 체크
            material = &m_materials[item.materialHandle - 1];

        if (!material)
        {
            if (m_defaultMaterial > 0 && m_defaultMaterial <= m_materials.size())
                material = &m_materials[m_defaultMaterial - 1];
        }


        RenderPassHandle passHandle = material->pass;

        if (passHandle == 0)
            passHandle = m_defaultPass;

        if (passHandle == 0 || passHandle > m_passes.size())
            continue;

        // 렌더 패스 바인드
        m_passes[passHandle - 1]->Bind(m_context.Get());

        // 메쉬 바인드
        const MeshResource& mesh = m_meshes[item.meshHandle - 1];
        mesh.Bind(m_context.Get());

        // 상수 버퍼 바인드
        BindConstantBuffers(item);

        // 텍스쳐 바인드
        BindTextures(*material);

        // 드로우
        if (mesh.ib && mesh.indexCount > 0)
            m_context->DrawIndexed(mesh.indexCount, 0, 0);
        else
            m_context->Draw(mesh.vertexCount, 0);
    }

    m_renderQueue.clear();
}

void YunoRenderer::RegisterDrawUI()
{
    ImGuiManager::RegisterDraw(
        [this]() {
            UI::BeginPanel("RenderOption");

            if (UI::CollapsingHeader("BloomFilter"))
            {
                UI::DragFloatEditable("Threshold", &m_Threshold, 0.01f, 0.0f, 1.5f);
                UI::DragFloatEditable("BloomIntensity", &m_BloomIntensity, 0.01f, 0.0f, 2.0f);
            }

            if (UI::CollapsingHeader("ToneMapping"))
            {
                UI::DragFloatEditable("Exposure", &m_Exposure, 0.01f, 0.0f, 5.0f);
            }

            UI::EndPanel();
        }
    );
}
#endif

MeshHandle YunoRenderer::CreateLineMesh_PosOnly(const VERTEX_Pos* verts, uint32_t vtxCount)
{
    if (!m_device || !verts || vtxCount == 0)
        return 0;

    MeshResource mr{};
    mr.flags = VSF_Pos;
    mr.vertexCount = vtxCount;
    mr.indexCount = 0;
    mr.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(VERTEX_Pos) * vtxCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = verts;

    if (FAILED(m_device->CreateBuffer(&bd, &init, mr.vbPos.ReleaseAndGetAddressOf())))
        return 0;

    m_meshes.push_back(std::move(mr));
    return static_cast<MeshHandle>(m_meshes.size()); // 1-based
}
