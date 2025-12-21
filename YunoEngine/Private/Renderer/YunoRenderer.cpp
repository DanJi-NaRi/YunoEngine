#include "pch.h"

#include "YunoRenderer.h"
#include "IWindow.h"

using Microsoft::WRL::ComPtr;

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

    return true;
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

    SetViewPort();                                                              // 뷰포트 설정

    const float clearColor[4] = { 0.05f, 0.1f, 0.2f, 1.0f }; // 어두운 파랑


    m_context->ClearRenderTargetView(m_backBufferRT.rtv.Get(), clearColor);                  // 화면 클리어

    ClearDepthStencil();                                                        // 뎁스/스텐실 클리어

}

void YunoRenderer::EndFrame()
{
    // VSync ON
    m_swapChain->Present(1, 0);
}

void YunoRenderer::Resize(uint32_t width, uint32_t height)
{
    if (!m_swapChain || !m_context || !m_device)
        return;

    if (width == 0 || height == 0)
        return;

    if (width == m_width && height == m_height)
        return;

    m_width = width;
    m_height = height;

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



