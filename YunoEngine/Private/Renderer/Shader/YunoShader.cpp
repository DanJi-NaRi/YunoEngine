#include "pch.h"

#include "YunoShader.h"



bool YunoShader::CreateVertexShader(ID3D11Device* device, ID3DBlob* bytecode)
{
    if (!device || !bytecode) return false;

    ComPtr<ID3D11VertexShader> vs;
    const HRESULT hr = device->CreateVertexShader(
        bytecode->GetBufferPointer(),
        bytecode->GetBufferSize(),
        nullptr,
        &vs);

    if (FAILED(hr)) return false;

    m_vs = std::move(vs);
    return true;
}

bool YunoShader::CreatePixelShader(ID3D11Device* device, ID3DBlob* bytecode)
{
    if (!device || !bytecode) return false;

    ComPtr<ID3D11PixelShader> ps;
    const HRESULT hr = device->CreatePixelShader(
        bytecode->GetBufferPointer(),
        bytecode->GetBufferSize(),
        nullptr,
        &ps);

    if (FAILED(hr)) return false;

    m_ps = std::move(ps);
    return true;
}
