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
    }

    void AttackAndMoveState::Update(PlayGridSystem* owner, float dt)
    {
        auto& us = owner->GetUtilitySequence();

        // 조건 충족 시 1회만 적용: 공격 SM이 Hit 상태 && 넉백/그랩 이동 존재
        if (!m_applied && owner->IsAttackInHitState() && us.hitMove != HitMove::None)
        {
            const auto& pieces = owner->GetAttackSequence().hitPieces;
            const auto& hm = us.hittersMove;
            if (pieces.size() != hm.size())
            {
                std::cout << "hitter count and hitter move ain't same!\n";
                assert(false);
            }
            for (int i = 0; i < pieces.size(); i++)
            {
                if (!owner->IsPieceNotDying(pieces[i]))    continue;
                owner->ApplyMoveInfo(hm[i].get());
            }
            m_applied = true;
        }

        m_elapsed += dt;
        if (m_elapsed >= us.m_attackAndMoveDuration)
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
