#pragma once

template <typename T>
class IState
{
public:
    virtual ~IState() = default;

    virtual void Enter(T* owner) = 0;
    virtual void Update(T* owner, float dt) = 0;
    virtual void Exit(T* owner) = 0;
};
