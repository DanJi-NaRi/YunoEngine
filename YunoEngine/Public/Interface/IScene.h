#pragma once
#include <memory>

class IRenderer;


class IScene
{
public:
    virtual ~IScene() = default;

    // 씬 활성화 >> 해당 씬에 필요한 오브젝트들 로드
    virtual void OnEnter() {}

    // 씬 비활성화 >> 로드한 오브젝트들 언로드
    virtual void OnExit() {}

    // 프레임 업데이트
    virtual void Update(float dt) = 0;

    // 렌더 아이템 제출
    virtual void Submit(IRenderer* renderer) = 0;
};

using ScenePtr = std::unique_ptr<IScene>;
