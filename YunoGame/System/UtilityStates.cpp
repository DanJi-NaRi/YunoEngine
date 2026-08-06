#include "pch.h"

#include "ObjectManager.h"
#include "GameManager.h"
#include "PlayQueue.h"

#include "UnitPiece.h"

#include "PlayGridSystem.h"
#include "UtilityStates.h"

namespace UtilityStates
{
    //--------------------------------------------------
    // MoveState : 사용 기물 본인의 이동 적용
    //--------------------------------------------------
    void MoveState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;

        const auto& us = owner->GetUtilitySequence();
        if (us.playerMove != nullptr)
            owner->ApplyMoveInfo(us.playerMove.get());
    }

    void MoveState::Update(PlayGridSystem* owner, float dt)
    {
        m_elapsed += dt;
        if (m_elapsed >= owner->GetUtilitySequence().m_moveDuration)
            owner->ChangeUtilityState(UtilityPhase::AttackAndMove);
    }

    void MoveState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // AttackAndMoveState : 공격 시퀀스가 Hit에 진입하면 피격자 넉백/그랩 이동 적용
    //--------------------------------------------------
    void AttackAndMoveState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;
        m_applied = false;

        // 이 상태에 도착하기 전에 이미 Hit 통지가 지나갔다면 여기서 회수한다.
        if (owner->HasAttackHitStarted())
            ApplyHittersMove(owner);
    }

    void AttackAndMoveState::ApplyHittersMove(PlayGridSystem* owner)
    {
        if (m_applied)  return;

        auto& us = owner->GetUtilitySequence();
        if (us.hitMove == HitMove::None)    return;     // 넉백/그랩이 없는 카드

        for (const auto& hitter : us.hittersMove)
        {
            if (hitter.move == nullptr)                     continue;
            if (!owner->IsPieceNotDying(hitter.piece))      continue;
            owner->ApplyMoveInfo(hitter.move.get());
        }

        m_applied = true;
    }

    void AttackAndMoveState::Update(PlayGridSystem* owner, float dt)
    {
        m_elapsed += dt;
        if (m_elapsed >= owner->GetUtilitySequence().m_attackAndMoveDuration)
            owner->ChangeUtilityState(UtilityPhase::Buff);
    }

    void AttackAndMoveState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // BuffState : 버프 이펙트/사운드 적용
    //--------------------------------------------------
    void BuffState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;

        auto& us = owner->GetUtilitySequence();
        if (us.buffData != nullptr)
            owner->PlayBuffEvent(us.playPiece, us.buffData);
    }

    void BuffState::Update(PlayGridSystem* owner, float dt)
    {
        m_elapsed += dt;
        if (m_elapsed >= owner->GetUtilitySequence().m_buffDuration)
            owner->ChangeUtilityState(UtilityPhase::Over);
    }

    void BuffState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // OverState : 시퀀스 정리 후 대기
    //--------------------------------------------------
    void OverState::Enter(PlayGridSystem* owner)
    {
        // MoveInfo는 unique_ptr가 자동 해제한다.
        owner->GetUtilitySequence() = {};
        owner->SetUtilityActive(false);
    }

    void OverState::Update(PlayGridSystem* owner, float dt)
    {
    }

    void OverState::Exit(PlayGridSystem* owner)
    {
    }
}
