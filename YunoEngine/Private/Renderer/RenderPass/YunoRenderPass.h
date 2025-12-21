#pragma once


struct ID3D11Device;
struct ID3D11DeviceContext;

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;

class YunoShader;

struct YunoRenderPassDesc
{
    // 셰이더
    YunoShader* vs = nullptr;
    YunoShader* ps = nullptr;

    // InputLayout 생성에 VS 바이트코드 필요
    ID3DBlob* vsBytecode = nullptr;

    const D3D11_INPUT_ELEMENT_DESC* inputElements = nullptr;
    uint32_t inputElementCount = 0;

    // 상태(최소)
    bool enableDepth = false;
    bool wireframe = false;
    bool enableBlend = false;
    bool cullBack = true; // true면 back-face cull
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

    bool m_depthEnabled = false;
    bool m_blendEnabled = false;
};
