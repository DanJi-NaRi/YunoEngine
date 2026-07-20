#pragma once
#include "../IState.h"

class PlayGridSystem;

// UtilityPhase(Move -> AttackAndMove -> Buff -> Over)를 IState로 옮긴 상태 클래스들.
// 상태 전환은 owner->ChangeUtilityState()를 통해서만 수행한다.
namespace UtilityStates
{
    class MoveState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;

    private:
        float m_elapsed = 0.f;
    };

    // 공격 시퀀스가 Hit 상태에 진입하고, 넉백/그랩 이동이 있을 때 피격자 이동을 1회 적용한다.
    class AttackAndMoveState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;

    private:
        float m_elapsed = 0.f;
        bool m_applied = false;     // 피격자 이동을 이미 적용했는가 (조건 충족 시 1회만)
    };

    class BuffState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;

    private:
        float m_elapsed = 0.f;
    };

    // 시퀀스 정리 후 머무는 휴지 상태.
    class OverState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;
    };
}
