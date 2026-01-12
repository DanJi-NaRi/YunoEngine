#pragma once


class YunoMeshBuffer
{
public:
    // VB
    bool CreateVertexOnly(
        ID3D11Device* device,
        const void* vertices,
        uint32_t strideBytes,
        uint32_t vertexCount,
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // VB + IB
    bool Create(
        ID3D11Device* device,
        const void* vertices,
        uint32_t strideBytes,
        uint32_t vertexCount,
        const uint16_t* indices,
        uint32_t indexCount,
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    void Bind(ID3D11DeviceContext* ctx) const;

    bool HasIndexBuffer() const { return m_ib != nullptr; }
    uint32_t GetVertexCount() const { return m_vertexCount; }
    uint32_t GetIndexCount() const { return m_indexCount; }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_ib;

    uint32_t m_strideBytes = 0;
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;

    D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
