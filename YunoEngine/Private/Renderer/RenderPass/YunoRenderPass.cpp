#include "pch.h"
#include "YunoRenderPass.h"

#include "YunoShader.h"



bool YunoRenderPass::Create(ID3D11Device* device, const YunoRenderPassDesc& desc)
{
    if (!device) return false;
    if (!desc.vs || !desc.ps) return false;
    if (!desc.vsBytecode) return false;
    if (!desc.inputElements || desc.inputElementCount == 0) return false;

    m_vs = desc.vs;
    m_ps = desc.ps;

    // InputLayout
    {
        Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
        const HRESULT hr = device->CreateInputLayout(
            desc.inputElements,
            desc.inputElementCount,
            desc.vsBytecode->GetBufferPointer(),
            desc.vsBytecode->GetBufferSize(),
            &layout);

        if (FAILED(hr)) return false;
        m_inputLayout = std::move(layout);
    }

    // RasterizerState
    {
        D3D11_RASTERIZER_DESC rd{};
        rd.FillMode = desc.wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
        rd.CullMode = desc.cullBack ? D3D11_CULL_BACK : D3D11_CULL_NONE;
        rd.FrontCounterClockwise = FALSE;
        rd.DepthClipEnable = TRUE;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rs;
        const HRESULT hr = device->CreateRasterizerState(&rd, &rs);
        if (FAILED(hr)) return false;
        m_rs = std::move(rs);
    }

    // DepthStencilState (MVP는 depth off 가능)
    {
        D3D11_DEPTH_STENCIL_DESC dd{};
        dd.DepthEnable = desc.enableDepth ? TRUE : FALSE;
        dd.DepthWriteMask = desc.enableDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        dd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dss;
        const HRESULT hr = device->CreateDepthStencilState(&dd, &dss);
        if (FAILED(hr)) return false;
        m_dss = std::move(dss);
    }

    // BlendState (기본: opaque)
    {
        D3D11_BLEND_DESC bd{};
        bd.AlphaToCoverageEnable = FALSE;
        bd.IndependentBlendEnable = FALSE;

        auto& rt = bd.RenderTarget[0];
        rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        if (desc.enableBlend)
        {
            rt.BlendEnable = TRUE;
            rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
            rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            rt.BlendOp = D3D11_BLEND_OP_ADD;
            rt.SrcBlendAlpha = D3D11_BLEND_ONE;
            rt.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        }

        Microsoft::WRL::ComPtr<ID3D11BlendState> bs;
        const HRESULT hr = device->CreateBlendState(&bd, &bs);
        if (FAILED(hr)) return false;
        m_bs = std::move(bs);
    }

    m_depthEnabled = desc.enableDepth;
    m_blendEnabled = desc.enableBlend;
    return true;
}

void YunoRenderPass::Bind(ID3D11DeviceContext* ctx)
{
    if (!ctx) return;

    // IA
    ctx->IASetInputLayout(m_inputLayout.Get());

    // Shaders
    ctx->VSSetShader(m_vs->VS(), nullptr, 0);
    ctx->PSSetShader(m_ps->PS(), nullptr, 0);

    // States
    ctx->RSSetState(m_rs.Get());
    ctx->OMSetDepthStencilState(m_dss.Get(), 0);

    const float blendFactor[4] = { 0,0,0,0 };
    ctx->OMSetBlendState(m_bs.Get(), blendFactor, 0xFFFFFFFF);
}
