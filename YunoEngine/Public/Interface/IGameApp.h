#pragma once

class IGameApp
{
public:
    virtual ~IGameApp() = default;

    virtual bool OnInit() = 0;
    virtual void OnUpdate(float dt) = 0;
    virtual void OnFixedUpdate(float fixedDt) = 0;
    virtual void OnShutdown() = 0;
};
