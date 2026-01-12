#include "pch.h"
#include "YunoMeshBuffer.h"

bool YunoMeshBuffer::CreateVertexOnly(
    ID3D11Device* device,
    const void* vertices,
    uint32_t strideBytes,
    uint32_t vertexCount,
    D3D11_PRIMITIVE_TOPOLOGY topology)
{
    if (!device || !vertices || strideBytes == 0 || vertexCount == 0)
        return false;

    m_strideBytes = strideBytes;
    m_vertexCount = vertexCount;
    m_indexCount = 0;
    m_topology = topology;

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = strideBytes * vertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = vertices;

    HRESULT hr = device->CreateBuffer(&bd, &init, m_vb.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        return false;

    m_ib.Reset();
    return true;
}

bool YunoMeshBuffer::Create(
    ID3D11Device* device,
    const void* vertices,
    uint32_t strideBytes,
    uint32_t vertexCount,
    const uint16_t* indices,
    uint32_t indexCount,
    D3D11_PRIMITIVE_TOPOLOGY topology)
{
    if (!device || !vertices || !indices)
        return false;
    if (strideBytes == 0 || vertexCount == 0 || indexCount == 0)
        return false;

    // 1) VB
    if (!CreateVertexOnly(device, vertices, strideBytes, vertexCount, topology))
        return false;

    // 2) IB
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(uint16_t) * indexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = indices;

    HRESULT hr = device->CreateBuffer(&bd, &init, m_ib.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        return false;

    m_indexCount = indexCount;
    return true;
}

void YunoMeshBuffer::Bind(ID3D11DeviceContext* ctx) const
{
    if (!ctx || !m_vb)
        return;

    const UINT stride = m_strideBytes;
    const UINT offset = 0;

    ctx->IASetVertexBuffers(0, 1, m_vb.GetAddressOf(), &stride, &offset);

    if (m_ib)
    {
        ctx->IASetIndexBuffer(m_ib.Get(), DXGI_FORMAT_R16_UINT, 0);
    }

    ctx->IASetPrimitiveTopology(m_topology);
}
