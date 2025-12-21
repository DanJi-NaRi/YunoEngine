#pragma once

#include "IRenderer.h"

enum ClearColor { Black, White, Gray, Red, Blue, Green, MAX_COUNT };

struct RenderTarget
{
    ComPtr<ID3D11Texture2D> tex;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11ShaderResourceView> srv;
    uint32_t w = 0, h = 0;
    DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
};

class IWindow;

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



};
