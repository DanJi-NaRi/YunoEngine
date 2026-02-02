#pragma once
#include "RenderTypes.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

class YunoShader;

enum class BlendMode : uint8_t { Opaque, AlphaBlend, ColorBlend, ColorBlendOne };
enum class FillMode : uint8_t { Solid, Wireframe };
enum class CullMode : uint8_t { None, Back, Front };
enum class DepthMode : uint8_t { Off, ReadWrite, ReadOnly };

struct YunoRenderPassDesc
{
    // 셰이더
    YunoShader* vs = nullptr;
    YunoShader* ps = nullptr;

    // InputLayout 생성에 VS 바이트코드 필요
    ID3DBlob* vsBytecode = nullptr;

    const D3D11_INPUT_ELEMENT_DESC* inputElements = nullptr;
    uint32_t inputElementCount = 0;


    bool wireframe = false;
    DepthMode depth = DepthMode::ReadWrite;
    CullMode  cull = CullMode::Back;
    BlendMode blend = BlendMode::Opaque;
    MaterialDomain domain = MaterialDomain::Surface;
};

class YunoRenderPass
{
public:
    bool Create(ID3D11Device* device, const YunoRenderPassDesc& desc);
    void Bind(ID3D11DeviceContext* ctx);

private:
    YunoShader* m_vs = nullptr;
    YunoShader* m_ps = nullptr;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>      m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>  m_rs;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dss;
    Microsoft::WRL::ComPtr<ID3D11BlendState>       m_bs;
};
