#include "pch.h"

#include "YunoRenderer.h"

// 셰이더
#include "YunoRenderPass.h"
//#include "YunoMeshBuffer.h"
#include "YunoShaderCompiler.h"
#include "YunoShader.h"



// 인터페이스
#include "IWindow.h"

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
    if (!m_cbDefault.Create(m_device.Get())) return false;
    if (!m_cbMaterial.Create(m_device.Get())) return false;


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

    // 디버그 리소스 생성
    CreateDebugGridResources();


    return true;
}

bool YunoRenderer::CreateShaders()
{
    // 여기서 쉐이더들 초기화 쭉 하면 됨
    if (!LoadShader(ShaderId::Basic, "../Assets/Shaders/BasicColor.hlsl", "VSMain", "PSMain")) return false;
    if (!LoadShader(ShaderId::DebugGrid, "../Assets/Shaders/DebugGrid.hlsl", "VSMain", "PSMain")) return false;
    
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

    if (FAILED(m_device->CreateRenderTargetView(
        m_backBufferTex.Get(), nullptr, m_backBufferRT.rtv.GetAddressOf())))
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
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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

        const DXGI_FORMAT colorFmt = DXGI_FORMAT_R8G8B8A8_UNORM;
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
        m_context->OMSetRenderTargets(1, m_backBufferRT.rtv.GetAddressOf(), m_dsv.Get());
    }        

    SetViewPort();                                                                           // 뷰포트 설정

    const float clearColor[4] = { 0.05f, 0.1f, 0.2f, 1.0f };                                 // 어두운 파랑

    // 화면 클리어
    if (m_msaaSamples > 1 && m_msaaRT.rtv)                  // MSAA 사용
        m_context->ClearRenderTargetView(m_msaaRT.rtv.Get(), clearColor);
    else                                                    // MSAA 미사용
        m_context->ClearRenderTargetView(m_backBufferRT.rtv.Get(), clearColor);   

    // 뎁스/스텐실 클리어
    ClearDepthStencil();                                                                     

}

void YunoRenderer::EndFrame()
{
    if (m_msaaSamples > 1 && m_msaaRT.tex && m_backBufferTex)
    {
        m_context->ResolveSubresource(
            m_backBufferTex.Get(), 0,
            m_msaaRT.tex.Get(), 0,
            DXGI_FORMAT_R8G8B8A8_UNORM);
    }

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

        const DXGI_FORMAT colorFmt = DXGI_FORMAT_R8G8B8A8_UNORM;
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
        const float border[4] = { 0,0,0,0 };
        m_samplers[(uint8_t)SampleMode::ClampAniso] = MakeAniso(D3D11_TEXTURE_ADDRESS_CLAMP, border);
        if (!m_samplers[(uint8_t)SampleMode::ClampAniso]) return false;
    }

    // 3) Border(0,0,0,0) + Aniso
    {
        const float border[4] = { 0,0,0,0 };
        m_samplers[(uint8_t)SampleMode::Border0000Aniso] = MakeAniso(D3D11_TEXTURE_ADDRESS_BORDER, border);
        if (!m_samplers[(uint8_t)SampleMode::Border0000Aniso]) return false;
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

MaterialHandle YunoRenderer::CreateMaterial(const MaterialDesc& desc)
{
    const RenderPassHandle pass = GetOrCreatePass(desc.passKey);
    if (pass == 0)
        return 0;

    YunoMaterial mat{};
    mat.pass = pass;

    mat.cpuParams.baseColor = desc.baseColor;
    mat.cpuParams.roughness = desc.roughness;
    mat.cpuParams.metallic = desc.metallic;

    mat.cpuParams.flags = 0;

    mat.albedo = desc.albedo;
    mat.normal = desc.normal;
    mat.orm = desc.orm;

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

    md.baseColor = { 1,1,1,1 };
    md.roughness = 1.0f;
    md.metallic = 0.0f;

    const MaterialHandle h = CreateMaterial(md);
    if (h == 0) return 0;

    m_defaultMaterial = h;
    return m_defaultMaterial;
}


TextureHandle YunoRenderer::CreateTexture2DFromFile(const wchar_t* path)
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

    // 3) WIC 로딩 (RGBA8)
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


RenderPassHandle YunoRenderer::GetOrCreatePass(const PassKey& key)
{
    auto it = m_passCache.find(key);
    if (it != m_passCache.end())
        return it->second; // 패스가 이미 있으면 그거 주고 없으면 새로만듬

    if (key.vs != key.ps)
        return 0;

    auto itProg = m_programs.find(key.vs);
    if (itProg == m_programs.end())
        return 0;

    const ShaderProgram& shader = itProg->second;
    if (!shader.vs || !shader.ps || !shader.vsBytecode)
        return 0;


    std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
    if (!InputLayoutFromFlags(key.vertexFlags, layout)) // 플래그에 맞는 layout을 만들어서 담아줌
        return 0;

    YunoRenderPassDesc pd{};
    pd.vs = shader.vs.get();                                            // 키 기반 완료
    pd.ps = shader.ps.get();                                            // 키 기반 완료
    pd.vsBytecode = shader.vsBytecode.Get();                            // 키 기반 완료
    pd.inputElements = layout.data();                                   // 키 기반 완료
    pd.inputElementCount = static_cast<uint32_t>(layout.size());        // 키 기반 완료

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
    // 0=Pos, 1=Nrm, 2=UV, 3=T, 4=B
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

    m_renderQueue.push_back(copy);
}

void YunoRenderer::Flush()
{
    if (!m_context || !m_cbDefault.IsValid() || !m_cbMaterial.IsValid())
        return;

    SubmitDebugGrid();

    // 렌더 전에 정렬 넣을예정

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
        BindConstantBuffers(item, *material);

        // 텍스쳐 바인드
        BindTextures(*material);

        // 샘플러 바인드
        BindSamplers();

        // 드로우
        if (mesh.ib && mesh.indexCount > 0)
            m_context->DrawIndexed(mesh.indexCount, 0, 0);
        else
            m_context->Draw(mesh.vertexCount, 0);
    }

    m_renderQueue.clear();
}

// ------------------------------------------------------------
// Bind 함수들
// ------------------------------------------------------------

void YunoRenderer::BindConstantBuffers(
    const RenderItem& item,
    const YunoMaterial& material
)
{
    using namespace DirectX;

    // -----------------------------
    // CBDefault (b0)
    // -----------------------------
    XMMATRIX W = XMLoadFloat4x4(&item.mWorld);
    XMMATRIX V = m_camera.View();
    XMMATRIX P = m_camera.Proj();
    XMMATRIX WVP = W * V * P;

    CBDefault cbd{};
    XMStoreFloat4x4(&cbd.mWorld, XMMatrixTranspose(W));
    XMStoreFloat4x4(&cbd.mView, XMMatrixTranspose(V));
    XMStoreFloat4x4(&cbd.mProj, XMMatrixTranspose(P));
    XMStoreFloat4x4(&cbd.mWVP, XMMatrixTranspose(WVP));

    m_cbDefault.Update(m_context.Get(), cbd);

    ID3D11Buffer* cbDefault[] = { m_cbDefault.Get() };
    m_context->VSSetConstantBuffers(0, 1, cbDefault);
    m_context->PSSetConstantBuffers(0, 1, cbDefault);




    // -----------------------------
    // CBMaterial (b1)
    // -----------------------------
    m_cbMaterial.Update(m_context.Get(), material.cpuParams);

    ID3D11Buffer* cbMaterial[] = { m_cbMaterial.Get() };
    m_context->VSSetConstantBuffers(1, 1, cbMaterial);
    m_context->PSSetConstantBuffers(1, 1, cbMaterial);
}

void YunoRenderer::BindSamplers()
{
    ID3D11SamplerState* samps[(uint8_t)SampleMode::Count] =
    {
        m_samplers[(uint8_t)SampleMode::Custom].Get(),
        m_samplers[(uint8_t)SampleMode::WrapAniso].Get(),
        m_samplers[(uint8_t)SampleMode::ClampAniso].Get(),
        m_samplers[(uint8_t)SampleMode::Border0000Aniso].Get(),
    };


    m_context->PSSetSamplers(0, (uint8_t)SampleMode::Count, samps);
}


void YunoRenderer::BindTextures(const YunoMaterial& material)
{
    TextureHandle hAlbedo = (material.albedo != 0) ? material.albedo : m_texWhite;
    TextureHandle hNormal = (material.normal != 0) ? material.normal : m_texNormal;
    TextureHandle hOrm = (material.orm != 0) ? material.orm : m_texBlack;

    ID3D11ShaderResourceView* srvs[3] =
    {
        ResolveSRV(hAlbedo),
        ResolveSRV(hNormal),
        ResolveSRV(hOrm) 
    };

    // 혹시라도 ResolveSRV가 nullptr이면 안전하게 nullptr 바인딩(디버그에서 보이게)
    m_context->PSSetShaderResources(0, 3, srvs);
}

// ------------------------------------------------------------
// Debug 함수들
// ------------------------------------------------------------

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
    md.passKey.depth = DepthPreset::ReadOnly;

    md.baseColor = { 1,1,0,1 };
    md.roughness = 1.0f;
    md.metallic = 0.0f;

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
    DirectX::XMStoreFloat4x4(&item.mWorld, DirectX::XMMatrixIdentity());
    Submit(item);
}

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
