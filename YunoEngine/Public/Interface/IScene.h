#pragma once

class IRenderer;

enum class SceneID : uint8_t
{
    Title,
    Play,
    UI
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

    virtual const char* GetDebugName() const = 0;
    virtual SceneID GetID() const = 0;
};
