#include "pch.h"

#include "ObjectManager.h"
#include "EffectManager.h"
#include "GameManager.h"
#include "AudioQueue.h"
#include "PlayQueue.h"

#include "EffectUnit.h"
#include "UnitTile.h"
#include "UnitPiece.h"

#include "PlayGridSystem.h"
#include "ObstacleStates.h"

namespace ObstacleStates
{
    //--------------------------------------------------
    // TriggerState : 경고 이펙트 제거 + 장애물 발동 + 피격/사망 처리
    //--------------------------------------------------
    void TriggerState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;

        auto& os = owner->GetObstacleSequence();
        auto pObjMng = owner->GetObjectManager();

        // 발동할 타일이 없으면 아무 것도 하지 않는다. (Update에서 즉시 Warning으로 전이)
        if (os.hitTileIDs.size() == 0)
            return;

        // 경고 이펙트 제거
        for (const auto& tileID : os.hitTileIDs)
        {
            auto& tileState = owner->GetTileState(tileID);
            if (tileState.effectIDs.size() == 0) continue;
            for (auto effectID : tileState.effectIDs)
            {
                pObjMng->DestroyObject(effectID);
            }
            tileState.effectIDs.clear();
        }

        // 장애물 발동
        for (const auto& tileID : os.hitTileIDs)
        {
            auto pTile = dynamic_cast<UnitTile*>(pObjMng->FindObject(owner->GetTileObjectID(tileID)));
            if (pTile == nullptr)   continue;
            if (pTile->IsCollapsed())    continue;

            pTile->PlayTrigger(os.attackType);

            if (os.attackType == ObstacleType::Collapse)
                owner->SetTileOccupyByID(tileID, TileOccupy{ TileOccuType::Collapesed, TileWho::None });
        }

        // 세로 장애물은 가운데 타일 위치에서 한번만 발동
        if (os.attackType == ObstacleType::Horizon_Razer)
        {
            int middleTileID = os.hitTileIDs[os.hitTileIDs.size() / 2];

            if (middleTileID != 0)
            {
                auto pTile = dynamic_cast<UnitTile*>(pObjMng->FindObject(owner->GetTileObjectID(middleTileID)));
                auto eff = owner->GetEffectManager()->Spawn(EffectID::Razer, { 0.f, 0.8f, 0.f }, { 11.f, 1.f, 1.f }, { -1, 0, 0 });
                if (pTile != nullptr && eff != nullptr)
                    pTile->Attach(eff);
            }

            AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_HorizonLazer));
        }

        // 기물 피격/사망 처리
        for (auto& piece : os.hitPieces)
        {
            const PieceInfo* pieceInfo = owner->FindPieceInfo(piece);
            if (pieceInfo == nullptr)   continue;

            int unitID = owner->GetUnitIDOf(piece);
            auto pPiece = dynamic_cast<UnitPiece*>(pObjMng->FindObject(pieceInfo->id));
            if (pPiece == nullptr)  continue;

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
        }

        std::cout << "[ObstacleStates] Obstacle is triggered\n";
    }

    void TriggerState::Update(PlayGridSystem* owner, float dt)
    {
        auto& os = owner->GetObstacleSequence();

        m_elapsed += dt;
        if (m_elapsed >= os.m_triggerDuration || os.hitTileIDs.size() == 0)
            owner->ChangeObstacleState(ObstaclePhase::Warning);
    }

    void TriggerState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // WarningState : 다음 턴 피격 타일 경고 연출
    //--------------------------------------------------
    void WarningState::Enter(PlayGridSystem* owner)
    {
        m_elapsed = 0.f;

        auto& os = owner->GetObstacleSequence();
        auto pObjMng = owner->GetObjectManager();

        // 경고할 타일이 없으면 아무 것도 하지 않는다. (Update에서 즉시 Over로 전이)
        if (os.warningTileIDs.size() == 0)
            return;

        AudioQ::Insert(AudioQ::PlayOneShot(EventName::UI_CointEvent));

        for (const auto& tileID : os.warningTileIDs)
        {
            auto pTile = dynamic_cast<UnitTile*>(pObjMng->FindObject(owner->GetTileObjectID(tileID)));
            if (pTile == nullptr)   continue;
            if (pTile->IsCollapsed())    continue;

            pTile->PlayWarning(os.attackType);

            EffectDesc ed{};
            ed.id = EffectID::FloorWarning1;
            ed.shaderid = ShaderId::EffectBase;
            ed.billboard = BillboardMode::None;
            ed.lifetime = 5.f;
            ed.framecount = 120;
            ed.cols = 12;
            ed.rows = 10;
            ed.emissive = 1.0f;
            ed.color = XMFLOAT4{ 1, 1, 0, 1 };
            ed.rot = { 0, 0, 0 };
            ed.isLoop = true;
            ed.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_1.png";
            if (os.attackType != ObstacleType::Collapse)
            {
                owner->GetEffectManager()->RegisterEffect(ed);
                auto pEffect1 = pObjMng->CreateObject<EffectUnit>(L"BarrierWarning1", XMFLOAT3(0, 0, -0.01f));
                pEffect1->BuildInternalEffectMaterial(ed);
                pTile->Attach(pEffect1);
                owner->GetTileState(tileID).effectIDs.push_back(pEffect1->GetID());
            }

            ed.id = EffectID::FloorWarning2;
            ed.framecount = 30;
            ed.lifetime = 1.2f;
            ed.cols = 5;
            ed.rows = 6;
            ed.color = (os.attackType == ObstacleType::Collapse) ? XMFLOAT4{ 1, 0, 0, 1 } : XMFLOAT4{ 1, 1, 0, 1 };
            ed.rot = { -XMConvertToRadians(90.f), 0, 0 };
            ed.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_2.png";
            owner->GetEffectManager()->RegisterEffect(ed);

            auto pEffect2 = pObjMng->CreateObject<EffectUnit>(L"BarrierWarning2", XMFLOAT3(0, 0, -0.3f));
            pEffect2->BuildInternalEffectMaterial(ed);
            pTile->Attach(pEffect2);
            owner->GetTileState(tileID).effectIDs.push_back(pEffect2->GetID());
        }

        std::cout << "[ObstacleStates] Warning Next Obstacle\n";
    }

    void WarningState::Update(PlayGridSystem* owner, float dt)
    {
        auto& os = owner->GetObstacleSequence();

        m_elapsed += dt;
        if (m_elapsed >= os.m_warningDuration || os.warningTileIDs.size() == 0)
            owner->ChangeObstacleState(ObstaclePhase::Over);
    }

    void WarningState::Exit(PlayGridSystem* owner)
    {
    }

    //--------------------------------------------------
    // OverState : 시퀀스 정리 후 대기
    //--------------------------------------------------
    void OverState::Enter(PlayGridSystem* owner)
    {
        owner->GetObstacleSequence() = {};
        owner->SetObstacleActive(false);
    }

    void OverState::Update(PlayGridSystem* owner, float dt)
    {
    }

    void OverState::Exit(PlayGridSystem* owner)
    {
    }
}
