#pragma once
#include <cstdint>
#include "RenderTypes.h"
#include "MaterialDesc.h"

class IWindow;
struct YunoCamera;

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

    // MSAA 단계 변경
    virtual bool SetMSAASamples(uint32_t samples) = 0; // 1/2/4/8
    virtual uint32_t GetMSAASamples() const = 0;

    // Vertex 스트림 + 인덱스로 GPU 메쉬 생성
    virtual MeshHandle CreateMesh(
        const VertexStreams& streams,
        const INDEX* triangles,
        uint32_t triCount) = 0;

    // 기본 머티리얼(추후 키/파일 기반으로 확장)
    virtual MaterialHandle CreateMaterial_Default() = 0;

    virtual MaterialHandle CreateMaterial(const MaterialDesc& desc) = 0;

    virtual TextureHandle CreateTexture2DFromFile(const wchar_t* path) = 0;

    // 매 프레임 렌더 요청 제출
    virtual void Submit(const RenderItem& item) = 0;

    // BeginFrame~EndFrame 사이에서 호출되어 실제 Draw 수행
    virtual void Flush() = 0;
    
    // 카메라 받아오기
    virtual YunoCamera& GetCamera() = 0;

    // 종료
    virtual void Shutdown() = 0;

};
