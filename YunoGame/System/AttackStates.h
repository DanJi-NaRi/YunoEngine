#pragma once
#include "../IState.h"

class PlayGridSystem;

// AttackPhase(Alaram -> Attack -> Hit -> Over)를 IState로 옮긴 상태 클래스들.
// 상태 전환은 owner->ChangeAttackState()를 통해서만 수행한다.
namespace AttackStates
{
    class AlarmState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;

    private:
        float m_elapsed = 0.f;
        bool m_aborted = false;     // 공격자가 사라졌을 때 Over로 즉시 전이
    };

    class AttackState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;

    private:
        float m_elapsed = 0.f;
        bool m_aborted = false;
    };

    class HitState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;

    private:
        float m_elapsed = 0.f;
    };

    // 시퀀스 정리 후 머무는 휴지 상태. 새 공격은 ChangeAttackState(AttackPhase::Alaram)으로 다시 시작한다.
    class OverState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;
    };
}
