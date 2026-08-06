#pragma once
#include "IState.h"

template <typename T>
class StateMachine
{
public:
    explicit StateMachine(T* owner) : m_owner(owner) {}

    void ChangeState(IState<T>* pNewState)
    {
        if (m_currentState != nullptr)
            m_currentState->Exit(m_owner);

        m_currentState = pNewState;

        if (m_currentState != nullptr)
            m_currentState->Enter(m_owner);
    }

    void Update(float dt)
    {
        if (m_currentState != nullptr)
            m_currentState->Update(m_owner, dt);
    }

    void Stop() { ChangeState(nullptr); }

    bool IsRunning() const { return m_currentState != nullptr; }
    bool IsInState(const IState<T>* pState) const { return m_currentState == pState; }

private:
    T* m_owner = nullptr;
    IState<T>* m_currentState = nullptr;
};
