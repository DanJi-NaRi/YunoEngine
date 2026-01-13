#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct CBDefault
{
    XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mWVP;
};

struct CBMaterial
{
    DirectX::XMFLOAT4 baseColor;
    float roughness;
    float metallic;
    uint32_t flags;
    float padding;
};


template<typename T>
class YunoConstantBuffer
{
public:
    bool Create(ID3D11Device* device)
    {
        if (!device) return false;

        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = Align16(sizeof(T));
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        return SUCCEEDED(device->CreateBuffer(&bd, nullptr, m_buffer.ReleaseAndGetAddressOf()));
    }

    void Update(ID3D11DeviceContext* ctx, const T& data)
    {
        if (!ctx || !m_buffer) return;

        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(ctx->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        {
            std::memcpy(ms.pData, &data, sizeof(T));
            ctx->Unmap(m_buffer.Get(), 0);
        }
    }

    ID3D11Buffer* Get() const { return m_buffer.Get(); }
    bool IsValid() const { return m_buffer != nullptr; }

private:
    static UINT Align16(size_t size)
    {
        return static_cast<UINT>((size + 15) & ~size_t(15));
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};
