#pragma once
#include <cstdint>
#include "RenderTypes.h"

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

    // Vertex 스트림 + 인덱스로 GPU 메쉬 생성
    virtual MeshHandle CreateMesh(
        const VertexStreams& streams,
        const uint32_t* indices,
        uint32_t indexCount) = 0;

    // 기본 머티리얼(추후 키/파일 기반으로 확장)
    virtual MaterialHandle GetOrCreateMaterial_Default() = 0;

    // 렌더패스 생성
    virtual RenderPassHandle CreateRenderPass(const RenderPassDesc& desc) = 0;

    // 매 프레임 렌더 요청 제출
    virtual void Submit(const RenderItem& item) = 0;

    // BeginFrame~EndFrame 사이에서 호출되어 실제 Draw 수행
    virtual void Flush() = 0;
    
    // 종료
    virtual void Shutdown() = 0;

};
