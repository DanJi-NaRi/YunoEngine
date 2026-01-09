#pragma once
#include <memory>

class IScene;
class IRenderer;

class ISceneManager
{
public:
    virtual ~ISceneManager() = default;

    // 씬 전환 요청
    virtual void RequestSetActive(std::unique_ptr<IScene> next) = 0;

    // 매 프레임 업데이트
    virtual void Update(float dt) = 0;

    // 매 프레임 렌더 제출
    virtual void Submit(IRenderer* renderer) = 0;

    // 디버깅,검사용
    virtual IScene* GetActive() const = 0;
};
