#pragma once

#include "IRenderer.h"
#include "YunoCamera.h"



enum ClearColor { Black, White, Gray, Red, Blue, Green, MAX_COUNT };

struct RenderTarget
{
    ComPtr<ID3D11Texture2D> tex;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11ShaderResourceView> srv;
    uint32_t w = 0, h = 0;
    DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
};


// 전방선언
class IWindow;
class YunoRenderPass;
class YunoMeshBuffer;
class YunoShader;

class YunoRenderer final : public IRenderer
{
public:
    YunoRenderer();
    ~YunoRenderer() override;

    bool Initialize(IWindow* window) override;
    void BeginFrame() override;
    void EndFrame() override;
    void Resize(uint32_t width, uint32_t height) override;
    void Shutdown() override;


    // 테스트 드로우 함수임 나중에 삭제 ㄱㄱ
    void RenderTestTriangle();

private:
    bool CreateDeviceAndSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    bool CreateRenderTarget();
    bool CreateDepthStencil(uint32_t width, uint32_t height);

    void SetViewPort();
    void ClearDepthStencil();



private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;

private:
    RenderTarget m_backBufferRT;

    Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_depthStencilTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbDefault;
    YunoCamera m_camera;
    float m_aspect = 1.0f;

private:
    // 셰이더
    std::unique_ptr<YunoRenderPass> m_basicPass;
    std::unique_ptr<YunoMeshBuffer> m_triangle;
    std::unique_ptr<YunoShader> m_basicVS;
    std::unique_ptr<YunoShader> m_basicPS;



};
