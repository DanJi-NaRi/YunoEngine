#pragma once
#include <cstdint>

class IWindow;

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    // DX11 장치/스왑체인 생성
    virtual bool Initialize(IWindow* window) = 0;

    // 매 프레임 시작
    virtual void BeginFrame() = 0;

    // 화면 표시
    virtual void EndFrame() = 0;

    // 창 크기 변경 대응
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    // 종료
    virtual void Shutdown() = 0;

};
