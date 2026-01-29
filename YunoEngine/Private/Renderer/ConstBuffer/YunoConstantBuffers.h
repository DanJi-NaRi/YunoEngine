#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct CBPerFrame
{
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
    XMFLOAT3 camPos;
    float pad = 0.0;
};

struct CBPerObject_Matrix
{
    XMFLOAT4X4 mWorld;
    XMFLOAT4X4 mWVP;
    XMFLOAT4X4 mWInvT;

    XMFLOAT4X4 mBoneAnim[MAX_BONE];
};

struct CBPerObject_Material
{
    XMFLOAT4 baseColor;
    XMFLOAT4 emissiveColor;
    float roughRatio;
    float metalRatio;
    float shadowBias;
    float opacity;
};

struct CBDirLight
{
    XMFLOAT4 dir;
    XMFLOAT4 diff;
    XMFLOAT4 amb;
    XMFLOAT4 spec;
    XMFLOAT4 intensity;// Only Use x
};

struct CBPointLight
{
    XMFLOAT4 col;
    XMFLOAT4 pos_intensity;//xyz pos, w intensity
};

struct CBLight_All
{
    CBDirLight dirLit;
    CBPointLight pointLit[10];
    XMFLOAT4 plCount = { 0, 0, 0, 1 }; // Only Use x
    //XMFLOAT4 padding[3];
};

struct CBPostProcess
{
    XMFLOAT2 InvScreenSize; // 1 / w, 1 / h
    float blurRadius = 2.0f;
    float threshold = 1.0f;

    float exposure = 1.0f;
    float padding[3]{ 0, 0, 0 };
};

struct CBBloom
{
    XMFLOAT4 bloomweights; //x : w0(1/2), y : w1(1/4), z : w2(1/8), w : w3(1/16)
    float bloomIntensity = 1.0f;
    XMFLOAT3 padding;
};

struct CBShadow
{
    XMFLOAT4X4 lightViewProj;
    float shadowMapSize;
    float shadowBias;
    float padding[2];
};

struct CBEffect
{
    XMFLOAT4 effectData; //x : index, y : time, z : cols, w : rows
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
