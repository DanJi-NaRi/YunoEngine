#pragma once
#include <cstdint>
#include "RenderTypes.h"
#include "MaterialDesc.h"

class IWindow;
class YunoMaterial;
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

    //기본 쿼드 메쉬핸들 받기
    virtual MeshHandle GetQuadMesh() = 0;

    virtual YunoMaterial& GetMaterial(MaterialHandle handle) = 0; //마테리얼 본체 못쓰면 텍스쳐 변경이 너무 까다로움

    // 기본 머티리얼(추후 키/파일 기반으로 확장)
    virtual MaterialHandle CreateMaterial_Default() = 0;

    virtual MaterialHandle CreateMaterial(const MaterialDesc& desc) = 0;

    virtual TextureHandle CreateColorTexture2DFromFile(const wchar_t* path) = 0; //Albedo, BaseColor용 -> 자동으로 Gpu에 넘길 때 감마커렉션 적용해줌
    virtual TextureHandle CreateDataTexture2DFromFile(const wchar_t* path) = 0; //Data 텍스쳐용 -> 원본 손상 X

    // 매 프레임 렌더 요청 제출
    virtual void Submit(const RenderItem& item) = 0;

    // BeginFrame~EndFrame 사이에서 호출되어 실제 Draw 수행
    virtual void DrawShadowMap() = 0;
    virtual void Flush() = 0;

#ifdef _DEBUG
    virtual void DrawDebug() = 0;
#endif

    virtual void PostProcess() = 0;

    // 프레임당 1회 상수 버퍼 업데이트
    virtual void BindConstantBuffers_Camera(const Frame_Data_Dir& dirData) = 0;
    virtual void BindConstantBuffers_Light(const Frame_Data_Dir& dirData, const std::vector<Frame_Data_Point>& plData, UINT plCount) = 0;
    
    // 카메라 받아오기
    virtual YunoCamera& GetCamera() = 0;

    //포스트프로세싱 설정
    virtual void SetPostProcessOption(uint32_t flag) = 0; //Use PostProcessFlag (기존 설정된 옵션 있으면 +, 없으면 Set)
    virtual void ResetPostProcessOption() = 0; //Reset PostProcessOption

    // 종료
    virtual void Shutdown() = 0;

};
