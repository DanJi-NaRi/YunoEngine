#pragma once
#include "../IState.h"

class PlayGridSystem;

// ObstaclePhase(Trigger -> Warning -> Over)를 IState로 옮긴 상태 클래스들.
// 상태 전환은 owner->ChangeObstacleState()를 통해서만 수행한다.
namespace ObstacleStates
{
    class TriggerState : public IState<PlayGridSystem>
    {
    public:
        void Enter(PlayGridSystem* owner) override;
        void Update(PlayGridSystem* owner, float dt) override;
        void Exit(PlayGridSystem* owner) override;

    private:
        float m_elapsed = 0.f;
    };

    class WarningState : public IState<PlayGridSystem>
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
