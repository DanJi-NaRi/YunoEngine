#pragma once

#include <string>

class IRenderer;
class SceneDesc;
class UIManager;

struct RuntimeSceneOverlayOptions
{
    // true면 현재 씬의 오브젝트/라이트를 전부 비우고 json 기준으로 재생성
    bool replaceObjects = true;

    // false면 카메라/후처리 적용을 건너뜀
    bool applyCamera = true;
    bool applyPostProcess = true;

    // false면 라이트(Dir/Point) 적용을 건너뜀
    bool applyLights = true;

    // 현재 WidgetDesc는 타입 정보가 없어 기본값 false 권장
    bool applyWidgets = false;
};

class IScene
{
public:
    virtual ~IScene() = default;


    virtual bool OnCreate() { return true; }    // 최초 1회
    virtual void OnEnter() {}                   // Active 직전/직후
    virtual void OnExit() {}                    // Active 해제 직전
    virtual void OnDestroy() {}                 // 제거 직전 1회

    virtual void OnPause() {}                   // 스택 위로 다른 씬이 올라왔을 때
    virtual void OnResume() {}                  // 다시 상단이 되었을 때


    virtual void Update(float dt) = 0;
    virtual void SubmitObj() = 0;              // 오브젝트들
    virtual void SubmitUI() = 0;                // UI들

    virtual UIManager* GetUIManager() = 0;

    virtual SceneDesc BuildSceneDesc() = 0;

    // 런타임에 외부 씬 json을 현재 씬에 오버레이
    virtual bool OverlaySceneFromFile(const std::wstring& filepath,
        const RuntimeSceneOverlayOptions& options = {}) = 0;


    virtual const char* GetDebugName() const = 0;
};
