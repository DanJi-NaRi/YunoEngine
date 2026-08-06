#include "pch.h"

#include "ObjectManager.h"
#include "EffectManager.h"
#include "GameManager.h"
#include "AudioQueue.h"
#include "PlayQueue.h"

#include "UnitTile.h"
#include "UnitPiece.h"

#include "PlayGridSystem.h"
#include "AttackStates.h"

namespace AttackStates
{
    //--------------------------------------------------
    // AlarmState : 공격자 회전 + 피격 예정 타일 플래시
    //--------------------------------------------------
    void AlarmState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;
        m_aborted = false;

        auto& as = owner->GetAttackSequence();

        const PieceInfo* pieceInfo = owner->FindPieceInfo(as.attacker);
        if (pieceInfo == nullptr)
        {
            m_aborted = true;
            return;
        }

        auto pPiece = dynamic_cast<UnitPiece*>(owner->GetObjectManager()->FindObject(pieceInfo->id));
        if (pPiece == nullptr)
        {
            m_aborted = true;
            return;
        }

        pPiece->SetDir(as.dir, true, owner->GetAttackRotDuration());

        for (int id : as.tileIDs)
        {
            auto pTile = dynamic_cast<UnitTile*>(owner->GetObjectManager()->FindObject(owner->GetTileObjectID(id)));
            if (pTile == nullptr)   continue;
            pTile->SetFlashColor(as.m_alarmColor, as.m_flashCount, as.m_flashInterval);
        }
    }

    void AlarmState::Update(PlayGridSystem* owner, float dt)
    {
        if (m_aborted)
        {
            owner->ChangeAttackState(AttackPhase::Over);
            return;
        }

        m_elapsed += dt;
        if (m_elapsed >= owner->GetAttackSequence().m_alarmDuration)
            owner->ChangeAttackState(AttackPhase::Attack);
    }

    void AlarmState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // AttackState : 공격 애니메이션 재생
    //--------------------------------------------------
    void AttackState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;
        m_aborted = false;

        auto& as = owner->GetAttackSequence();

        const PieceInfo* pieceInfo = owner->FindPieceInfo(as.attacker);
        if (pieceInfo == nullptr)
        {
            m_aborted = true;
            return;
        }

        auto pPiece = dynamic_cast<UnitPiece*>(owner->GetObjectManager()->FindObject(pieceInfo->id));
        if (pPiece == nullptr)
        {
            m_aborted = true;
            return;
        }

        pPiece->InsertQ({ CommandType::Attack });

        std::cout << "[Attack Sequence]\nAttacker hp: "
            << static_cast<int>(owner->GetUnitState(owner->GetUnitIDOf(as.attacker)).hp) << std::endl;
    }

    void AttackState::Update(PlayGridSystem* owner, float dt)
    {
        if (m_aborted)
        {
            owner->ChangeAttackState(AttackPhase::Over);
            return;
        }

        m_elapsed += dt;
        if (m_elapsed >= owner->GetAttackSequence().m_attackDuration)
            owner->ChangeAttackState(AttackPhase::Hit);
    }

    void AttackState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // HitState : 타일 피격 이펙트 + 피격/사망 애니메이션
    //--------------------------------------------------
    void HitState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;

        auto& as = owner->GetAttackSequence();
        auto pObjMng = owner->GetObjectManager();

        // 타일 피격 이펙트 시작
        const PieceInfo* attackerInfo = owner->FindPieceInfo(as.attacker);
        Team team = (attackerInfo != nullptr) ? attackerInfo->team : Team::Undefined;

        for (int id : as.tileIDs)
        {
            auto pTile = dynamic_cast<UnitTile*>(pObjMng->FindObject(owner->GetTileObjectID(id)));
            if (pTile == nullptr)   continue;

            Effect* eff = nullptr;
            int pid = GameManager::Get().GetPID();
            if ((pid == 1 && team == Team::Ally) || (pid == 2 && team == Team::Enemy))
                eff = owner->GetEffectManager()->Spawn(EffectID::Target, { 0, 0.01, 0 }, { 1, 1, 1 });
            else if ((pid == 1 && team == Team::Enemy) || (pid == 2 && team == Team::Ally))
                eff = owner->GetEffectManager()->Spawn(EffectID::TargetEnemy, { 0, 0.01, 0 }, { 1, 1, 1 });

            if (eff)
                pTile->Attach(eff);
        }

        AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_TileHit));

        // 기물 피격/사망 애니메이션 시작
        for (const auto& piece : as.hitPieces)
        {
            const PieceInfo* pieceInfo = owner->FindPieceInfo(piece);
            if (pieceInfo == nullptr) continue;

            auto pPiece = dynamic_cast<UnitPiece*>(pObjMng->FindObject(pieceInfo->id));
            if (pPiece == nullptr) continue;

            int unitID = owner->GetUnitIDOf(piece);
            if (owner->GetUnitState(unitID).hp == 0)
            {
                pPiece->InsertQ(PlayGridQ::Dead_P(owner->GetDisappearDissolveDuration()));
                for (auto& subId : pieceInfo->subIds)
                {
                    auto pSub = dynamic_cast<UnitPiece*>(pObjMng->FindObject(subId));
                    if (pSub == nullptr)    continue;
                    pSub->InsertQ(PlayGridQ::Dead_P(owner->GetDisappearDissolveDuration()));
                }
            }
            else
            {
                pPiece->InsertQ(PlayGridQ::Hit_P());
                for (auto& subId : pieceInfo->subIds)
                {
                    auto pSub = dynamic_cast<UnitPiece*>(pObjMng->FindObject(subId));
                    if (pSub == nullptr)    continue;
                    pSub->InsertQ(PlayGridQ::Hit_P());
                }
            }
            std::cout << "[AttackStates::HitState]\nHitter hp: "
                << static_cast<int>(owner->GetUnitState(unitID).hp) << std::endl;
        }

        // Utility 시퀀스에 Hit 진입을 통지한다. (넉백/그랩 이동 트리거)
        owner->OnAttackHitStarted();
    }

    void HitState::Update(PlayGridSystem* owner, float dt)
    {
        m_elapsed += dt;
        if (m_elapsed >= owner->GetAttackSequence().m_hitDuration)
            owner->ChangeAttackState(AttackPhase::Over);
    }

    void HitState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // OverState : 시퀀스 정리 후 대기
    //--------------------------------------------------
    void OverState::Enter(PlayGridSystem* owner)
    {
        owner->GetAttackSequence() = {};
        owner->SetAttackActive(false);
    }

    void OverState::Update(PlayGridSystem* owner, float dt)
    {
    }

    void OverState::Exit(PlayGridSystem* owner)
    {
    }
}
