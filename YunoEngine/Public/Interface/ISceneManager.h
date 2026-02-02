#pragma once
#include <cstdint>
#include <memory>

class IScene;
class IRenderer;

// 씬 상태
enum class SceneState : uint8_t
{
    Uninitialized = 0, // OnCreate 전
    Loaded,            // OnCreate 후
    Active,            // OnEnter 후
    Paused             // OnPause 후
};


struct ScenePolicy
{
    // 이 씬이 활성화 됐을 때 스택 하단에 존재하는 다른 씬들의 업데이트를 막을것인지
    bool blockUpdateBelow = false;

    // 이 씬이 활성화 됐을 때 스택 하단에 존재하는 다른 씬들의 렌더를 막을것인지
    bool blockRenderBelow = false;
};


struct SceneTransitionOptions
{
    bool immediate = true;      // true면 이번 프레임 끝에서 바로 전환 적용
};

class ISceneManager
{
public:
    virtual ~ISceneManager() = default;


    virtual void RequestReplaceRoot(std::unique_ptr<IScene> scene,
        SceneTransitionOptions opt = {}) = 0;

    virtual void RequestPush(std::unique_ptr<IScene> scene,
        ScenePolicy policy = {},
        SceneTransitionOptions opt = {}) = 0;

    virtual void RequestPop(SceneTransitionOptions opt = {}) = 0;


    virtual void Update(float dt) = 0;
    virtual void SubmitAndRender(IRenderer* renderer) = 0;


    virtual IScene* GetActiveScene() const = 0;
    virtual uint32_t GetStackSize() const = 0;
};
