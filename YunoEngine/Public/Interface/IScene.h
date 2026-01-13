#pragma once

class IRenderer;

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
    virtual void Submit(IRenderer* renderer) = 0;

    virtual const char* GetDebugName() const = 0;
};
