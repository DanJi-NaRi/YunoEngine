#pragma once

class IGameApp
{
public:
    virtual ~IGameApp() = default;

    // 게임 초기화 실패하면 false
    virtual bool OnInit() = 0;

    // 매 프레임 갱신
    virtual void OnUpdate(float dt) = 0;

    // 매 프레임 렌더
    virtual void OnRender() = 0;

    // 종료 처리
    virtual void OnShutdown() = 0;
};
