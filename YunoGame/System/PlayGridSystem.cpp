#include "pch.h"

#include "GridFactory.h"
#include "PlayQueue.h"

#include "ObjectManager.h"
#include "EffectManager.h"
#include "GameManager.h"
#include "AudioQueue.h"

#include "EffectUnit.h"
#include "Grid.h"
#include "UnitGridBox.h"
#include "UnitGridLine.h"
#include "UnitTile.h"
#include "UnitPiece.h"

#include "PacketBuilder.h"

#include "PlayGridSystem.h"


PlayGridSystem::PlayGridSystem(ObjectManager* objmng, EffectManager* effectmng) : UnitGridSystem(objmng), m_effectManager(effectmng)
{
    m_playQ = std::make_unique<PlayGridQ>();

    Init();
}

PlayGridSystem::~PlayGridSystem()
{
}

void PlayGridSystem::Init()
{
    for (int i = 0; i < static_cast<int>(NG_P::Max); i++)
    {
        switch (static_cast<NG_P>(i))
        {
        case NG_P::One:
        {
            CreateGrid(5, 7, 1.0721, 1.18713);
            break;
        }
        }
    }

    // m_tiles 초기화
    // 현재 단일 그리드라 m_tiles도 단일 벡터로 존재
    // 복수 그리드가 되는 그리드 시스템에서는 m_tiles는 unordered_map 혹은 벡터의 벡터로 존재해야함
    m_nowG = NG_P::One;
    SetNG_P(m_grids[(int)m_nowG].get());

    m_tiles = std::vector(m_column * m_row + 1, TileState{});
}

void PlayGridSystem::InitRound()
{
    const auto wData = GameManager::Get().GetWeaponData();
    GameManager::Get().ResetWeaponData();
    for (int i = 0; i < wData.size(); i++)
    {
        const auto& w = wData[i];
        int unitId = GetUnitID(w.pId, w.slotId);
        auto& us = m_UnitStates[unitId];
    
        us.hp = w.hp;
        us.stamina = w.stamina;
        us.targetTileID = w.currentTile;
        us.isEvent = 0;
    
        m_tiles[w.currentTile].te;  // 타일 이벤트 상태
    
        // 죽은 애도 살려내기
        GamePiece gp = GetGamePiece(w.pId, w.slotId);
        auto it = m_pieces.find(gp);
        if (it == m_pieces.end())
        {
            CreatePiece(w);
        }
        else // 문제 안 생기는 거 확인하고 지울 예정
        {
            std::cout << "Before Round's Pieces ain't removed!!\n";
            //assert(false);
            Direction dir = (w.pId == 1) ? Direction::Right : Direction::Left;
            auto pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(it->second.id));
            pPiece->SetDir(dir, false);
    
            // 기물 방향 정보 갱신
            m_pieces[gp].dir = dir;
    
            // 타일 정보 갱신
            m_tiles[w.currentTile].to = (m_pID == w.pId) ?
                TileOccupy{ TileOccuType::Ally_Occupied, (w.slotId == 1) ? TileWho::Ally1 : TileWho::Ally2 } :
                TileOccupy{ TileOccuType::Enemy_Occupied, (w.slotId == 1) ? TileWho::Enemy1 : TileWho::Enemy2 };
        }

        int usID = GetUnitID(w.pId, w.slotId);
        m_UnitStates[usID] = {
            (uint8_t)w.pId, (uint8_t)w.slotId, (uint8_t)w.hp,
            (uint8_t)w.stamina, (uint8_t)w.currentTile, 0
        };
        m_weaponIDs[i] = w.weaponId;
    }  
    ReflectWeaponData();
    ApplyTransform();
    isRoundOver = false;
}

void PlayGridSystem::Update(float dt)
{
    if (!GameManager::Get().IsEmptyWeaponData())    
        InitRound();       

    ReflectWeaponData();        // 이걸 여기 했는데 UI반영이 안된다? 그럼 뭔가 큰 일이 났다는 사실~

    UpdateSequence(dt);         // 카드 타입별 step 진행

    CheckPacket(dt);
    CheckMyQ();
}

void PlayGridSystem::ApplyTransform()
{
    for (auto&[piece, unit] : m_units)
    {
        auto id = unit->GetWeaponID();
        if (id == 3)
            unit->SetScale({ 2, 2, 2 });
        else
            unit->SetScale({ 1, 1, 1 });
    }
}

void PlayGridSystem::CreateObject(float x, float y, float z)
{
    CreateGridBox(x, y, z);

    // 그리드 생성 시, factory 초기화 필요
    // 복수 그리드 생성 시 해당 그리드 정보로 factory 초기화 후, [x y z] 간격 설정하여 CreateGridLine() 호출해주기
    //m_gridFactory->Init(m_grids[(int)m_nowG]->m_row, m_grids[(int)m_nowG]->m_column);
    //CreateGridLine(x, y, z);

    CreateTileAndPiece(x, y, z);
}

void PlayGridSystem::CreateTileAndPiece(float x, float y, float z)
{
    if (m_gridBox == nullptr) return;

    // 서버와 수 체계를 맞추기 위해.
    m_tilesIDs.push_back({});

    // 지금은 타일 매쉬와 메테리얼을 클래스 내부에서 수동으로 넣어주고 있지만
    // 후에 아트로부터 리소스를 받으면 CreateObject -> CreateObjectFromFile로 바뀌며 자동으로 로드됩니다.
    const int floorOfTile[35] = {
        5,10,15,20,25,30,1,
        4,9,14,19,24,29,2,
        3,8,13,18,23,28,3,
        2,7,12,17,22,27,4,
        1,6,11,16,21,26,5
    };
    for (int i = 0; i < m_tiles.size() - 1; i++)
    {
        int tilenum = floorOfTile[i];
        auto [wx, wz] = m_grids[(int)m_nowG]->CellToWorld(i % m_column, i / m_column);
        auto fileName = GetTileFileName(tilenum);
        auto pTile = m_manager->CreateObjectFromFile<UnitTile>(L"Tile", XMFLOAT3(wx, y, wz), fileName);
        pTile->SetRot({0, XMConvertToRadians(90.f), 0});
        if(tilenum > 0 && tilenum < 6)  // 1~5번 타일만
            SetTileInitState(pTile, tilenum);
        m_tilesIDs.push_back(pTile->GetID());

        // 빈 박스에 자식 객체로 등록. (for 정리)
        m_gridBox->Attach(pTile);
    }

    // 장애물 경고 이펙트 생성



    // 기물 생성
    const auto wData = GameManager::Get().GetWeaponData();
    GameManager::Get().ResetWeaponData();
    m_wy = y + 0.1f;
    int cx = 0; int cy = 0;
    Team team = Team::Undefined;
    Direction dir;
    m_pID = GameManager::Get().GetSlotiIdx();   // 나 몇번째 플레이어니?
    for (int i = 0; i < wData.size(); i++)
    {
        const auto& w = wData[i];
        CreatePiece(w);

        int usID = GetUnitID(w.pId, w.slotId);
        m_UnitStates[usID] = {
            (uint8_t)w.pId, (uint8_t)w.slotId, (uint8_t)w.hp,
            (uint8_t)w.stamina, (uint8_t)w.currentTile, 0
        };
        m_weaponIDs[i] = w.weaponId;
    }

    ReflectWeaponData();
}

void PlayGridSystem::CreatePiece(const Wdata& w)
{
    int cx = 0; int cy = 0;
    Team team = Team::Undefined;
    Direction dir;

     team = (w.pId == m_pID) ? Team::Ally : Team::Enemy;

     dir = (w.pId == 1) ? Direction::Right : Direction::Left;
     auto cellPos = GetCellByID(w.currentTile);
     cx = cellPos.x;     cy = cellPos.y;

     auto [px, pz] = m_grids[(int)m_nowG]->CellToWorld(cx, cy);
     GamePiece gp = GetGamePiece(w.pId, w.slotId);

     // 타일 상태 갱신
     m_tiles[w.currentTile].to =
         (gp == GamePiece::Ally1) ? TileOccupy{ TileOccuType::Ally_Occupied, TileWho::Ally1 } :
         (gp == GamePiece::Ally2) ? TileOccupy{ TileOccuType::Ally_Occupied, TileWho::Ally2 } :
         (gp == GamePiece::Enemy1) ? TileOccupy{ TileOccuType::Enemy_Occupied, TileWho::Enemy1 } :
         TileOccupy{ TileOccuType::Enemy_Occupied, TileWho::Enemy2 };

     PieceInfo pieceInfo{};
     PassOption po;
     po.shader = ShaderId::PBR_AniDissolve;
     UnitPiece* pPiece = nullptr;
     if (w.weaponId == 2)
     {
         pPiece = m_manager->CreateObject<UnitPiece>(
             L"Weapon", XMFLOAT3(px, m_wy, pz));
         //pPiece->SetMoveRotOffset(0, 0);
         //pPiece->SetIgnoreRot(true);

         auto pChakram1 = m_manager->CreateObjectFromFile<UnitPiece>(
             L"Chakram1", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Chakram/Chakram01.fbx", po);
         pChakram1->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/chakram_01_idle.fbx");
         pChakram1->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Chakram_attack_01.fbx");
         pChakram1->AddAnimationClip("move", L"../Assets/fbx/Animation/move/Chakram_move_01.fbx");
         pChakram1->AddAnimationClip("dead", L"../Assets/fbx/Animation/dead/Chakram_dead_01.fbx");
         pChakram1->AddAnimationClip("deadF", L"../Assets/fbx/Animation/dead/flip/Chakram_dead_flip_01.fbx");
         pChakram1->AddAnimationClip("hit", L"../Assets/fbx/Animation/hit/Chakram_hit_01.fbx");
         pChakram1->AddAnimationClip("hitF", L"../Assets/fbx/Animation/hit/flip/Chakram_hit_flip_01.fbx");
         pChakram1->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
         pChakram1->SetDissolveColor(XMFLOAT3(1, 1, 1));
         pChakram1->AppearDissolve(appearDisolveDuration);
         pChakram1->SetWho(gp, team, w.pId, w.weaponId, 1);
         pChakram1->SetDir(dir, false);
         pChakram1->SetMoveRotOffset(0, 0);
         pChakram1->SetIgnoreRot(true);

         pPiece->Attach(pChakram1);
         pPiece->AddLinkedSubPiece(pChakram1);

         auto pChakram2 = m_manager->CreateObjectFromFile<UnitPiece>(
             L"Chakram2", XMFLOAT3(0, 0, 0), L"../Assets/fbx/weapon/Chakram/Chakram02.fbx", po);
         pChakram2->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/chakram_02_idle.fbx");
         pChakram2->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Chakram_attack_02.fbx");
         pChakram2->AddAnimationClip("move", L"../Assets/fbx/Animation/move/Chakram_move_02.fbx");
         pChakram2->AddAnimationClip("dead", L"../Assets/fbx/Animation/dead/Chakram_dead_02.fbx");
         pChakram2->AddAnimationClip("deadF", L"../Assets/fbx/Animation/dead/flip/Chakram_dead_flip_02.fbx");
         pChakram2->AddAnimationClip("hit", L"../Assets/fbx/Animation/hit/Chakram_hit_02.fbx");
         pChakram2->AddAnimationClip("hitF", L"../Assets/fbx/Animation/hit/flip/Chakram_hit_flip_02.fbx");
         pChakram2->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
         pChakram2->SetDissolveColor(XMFLOAT3(1, 1, 1));
         pChakram2->AppearDissolve(appearDisolveDuration);
         pChakram2->SetWho(gp, team, w.pId, w.weaponId, 2);
         pChakram2->SetDir(dir, false);
         pChakram2->SetMoveRotOffset(0, 0);
         pChakram2->SetIgnoreRot(true);

         pPiece->Attach(pChakram2);
         pPiece->AddLinkedSubPiece(pChakram2);

         pieceInfo = PieceInfo{ pPiece->GetID(), dir, team };
         pieceInfo.subIds.push_back(pChakram1->GetID());
         pieceInfo.subIds.push_back(pChakram2->GetID());

         m_gridBox->Attach(pPiece);
     }
     else
     {
         std::wstring fileName = GetWeaponFileName(w.weaponId);
         pPiece = m_manager->CreateObjectFromFile<UnitPiece>(L"Weapon", XMFLOAT3(px, m_wy, pz), fileName, po);
         pieceInfo = PieceInfo{ pPiece->GetID(), dir, team };
     }

     if (pPiece == nullptr)
     {
         assert(false);
         std::cout << "[PlayGridSystem]::CreatePiece failed to create object\n";
         return;
     }

     switch (w.weaponId)
     {

     case 1:
         pPiece->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/blaster_idle.fbx");
         pPiece->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Blaster_attack.fbx");
         pPiece->AddAnimationClip("move", L"../Assets/fbx/Animation/move/Blaster_move.fbx");
         pPiece->AddAnimationClip("dead", L"../Assets/fbx/Animation/dead/Blaster_dead.fbx");
         pPiece->AddAnimationClip("deadF", L"../Assets/fbx/Animation/dead/flip/Blaster_dead_flip.fbx");
         pPiece->AddAnimationClip("hit", L"../Assets/fbx/Animation/hit/Blaster_hit.fbx");
         pPiece->AddAnimationClip("hitF", L"../Assets/fbx/Animation/hit/flip/Blaster_hit_flip.fbx");
         pPiece->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
         pPiece->SetDissolveColor(XMFLOAT3(1, 1, 1));

         break;
     case 3:
         pPiece->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/Breacher_idle.fbx");
         pPiece->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Breacher_attack.fbx");
         pPiece->AddAnimationClip("move", L"../Assets/fbx/Animation/move/Breacher_move.fbx");
         pPiece->AddAnimationClip("dead", L"../Assets/fbx/Animation/dead/Breacher_dead.fbx");
         pPiece->AddAnimationClip("deadF", L"../Assets/fbx/Animation/dead/Breacher_dead.fbx");
         pPiece->AddAnimationClip("hit", L"../Assets/fbx/Animation/hit/Breacher_hit.fbx");
         pPiece->AddAnimationClip("hitF", L"../Assets/fbx/Animation/hit/Breacher_hit.fbx");
         pPiece->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
         pPiece->SetDissolveColor(XMFLOAT3(1, 1, 1));
         pPiece->SetScale(XMFLOAT3(2, 2, 2));
         pPiece->SetMoveRotOffset(-0.25f, 0);
         break;
     case 4:
         pPiece->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/scythe_idle.fbx");
         pPiece->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/scythe_attack.fbx");
         pPiece->AddAnimationClip("move", L"../Assets/fbx/Animation/move/scythe_move.fbx");
         pPiece->AddAnimationClip("dead", L"../Assets/fbx/Animation/dead/Scythe_dead.fbx");
         pPiece->AddAnimationClip("deadF", L"../Assets/fbx/Animation/dead/flip/Scythe_dead_flip.fbx");
         pPiece->AddAnimationClip("hit", L"../Assets/fbx/Animation/hit/Scythe_hit.fbx");
         pPiece->AddAnimationClip("hitF", L"../Assets/fbx/Animation/hit/flip/Scythe_hit_flip.fbx");
         pPiece->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
         pPiece->SetDissolveColor(XMFLOAT3(1, 1, 1));
         break;
     case 5:
         pPiece->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/impactor_idle.fbx");
         pPiece->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Impactor_attack.fbx");
         pPiece->AddAnimationClip("move", L"../Assets/fbx/Animation/move/Impactor_move.fbx");
         pPiece->AddAnimationClip("dead", L"../Assets/fbx/Animation/dead/Impactor_dead.fbx");
         pPiece->AddAnimationClip("deadF", L"../Assets/fbx/Animation/dead/flip/Impactor_dead_flip.fbx");
         pPiece->AddAnimationClip("hit", L"../Assets/fbx/Animation/hit/Impactor_hit.fbx");
         pPiece->AddAnimationClip("hitF", L"../Assets/fbx/Animation/hit/flip/Impactor_hit_flip.fbx");
         pPiece->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
         pPiece->SetDissolveColor(XMFLOAT3(1, 1, 1));
         break;
     case 6:
         pPiece->AddAnimationClip("idle", L"../Assets/fbx/Animation/idle/Cleaver_idle.fbx");
         pPiece->AddAnimationClip("attack", L"../Assets/fbx/Animation/attack/Cleaver_attack.fbx");
         pPiece->AddAnimationClip("move", L"../Assets/fbx/Animation/move/Cleaver_move.fbx");
         pPiece->AddAnimationClip("dead", L"../Assets/fbx/Animation/dead/Cleaver_dead.fbx");
         pPiece->AddAnimationClip("deadF", L"../Assets/fbx/Animation/dead/flip/Cleaver_dead_flip.fbx");
         pPiece->AddAnimationClip("hit", L"../Assets/fbx/Animation/hit/Cleaver_hit.fbx");
         pPiece->AddAnimationClip("hitF", L"../Assets/fbx/Animation/hit/flip/Cleaver_hit_flip.fbx");
         pPiece->SetNoiseTexture(L"../Assets/Textures/BloodDisolve.png");
         pPiece->SetDissolveColor(XMFLOAT3(1, 1, 1));
         break;
     }
     pPiece->SetWho(gp, team, w.pId, w.weaponId);
     pPiece->SetDir(dir, false);

     // 기물 따라다니는 바닥 이펙트
     EffectDesc ed{};
     ed.id = (w.pId == 1) ? EffectID::PeacePosAlly : EffectID::PeacePosEnemy;
     ed.shaderid = ShaderId::EffectBase;
     ed.billboard = BillboardMode::None;
     ed.lifetime = 1.5f;
     ed.framecount = 60;
     ed.cols = 8;
     ed.rows = 8;
     ed.emissive = 30.0f;
     ed.color = (w.pId == 1)? XMFLOAT4{ 0, 0.5f, 1, 1 } : XMFLOAT4{ 1, 0, 0, 1 };
     ed.rot = { XM_PIDIV2, 0, 0 };
     ed.isLoop = true;
     ed.texPath = (w.pId == 1) ? L"../Assets/Effects/Pos/EF_Player_Blue.png" : L"../Assets/Effects/Pos/EF_Player_Red.png";
     m_effectManager->RegisterEffect(ed);
     auto pEffect = m_manager->CreateObject<EffectUnit>(L"PeacePosAlly", XMFLOAT3(0, 0.01f, 0));
     pEffect->BuildInternalEffectMaterial(ed);
     pEffect->IgnoreRotation(1, 1, 1);
     pEffect->IgnoreScale(1, 1, 1);
     pieceInfo.effectIds.push_back(pEffect->GetID());
     pPiece->Attach(pEffect);

     // 디졸브 나타남
     pPiece->AppearDissolve(appearDisolveDuration);

     // 기물 정보 등록
     m_pieces.emplace(gp, pieceInfo);

     // 빈 박스에 자식 객체로 등록. (for 정리)
     m_gridBox->Attach(pPiece);
     m_units.emplace(gp, pPiece);
}

void PlayGridSystem::CheckMyQ()
{
    if (!m_playQ->Empty())
    {
        PGridCmd cmd = m_playQ->Pop();

        switch (cmd.cmdType)
        {
        case CommandType::Hit:
        {
            const GamePiece pieceType = cmd.hit_s.whichPiece;

            auto it = m_pieces.find(pieceType);
            if (it == m_pieces.end()) break;

            auto& pieceInfo = it->second;
            auto pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));

            int uid = GetUnitID(pieceType);
            if (static_cast<int>(m_UnitStates[uid].hp) <= 0)
            {
                pPiece->InsertQ(PlayGridQ::Dead_P(disappearDisolveDuration));

                for (uint32_t subId : pieceInfo.subIds)
                {
                    auto pSubPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(subId));
                    if (pSubPiece != nullptr)
                        pSubPiece->InsertQ(PlayGridQ::Dead_P(disappearDisolveDuration));
                }

            }
            else
            {
                pPiece->InsertQ(PlayGridQ::Hit_P());

                for (uint32_t subId : pieceInfo.subIds)
                {
                    auto pSubPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(subId));
                    if (pSubPiece != nullptr)
                        pSubPiece->InsertQ(PlayGridQ::Hit_P());
                }
            }

            std::cout << static_cast<int>(pieceType) << ". health: " << static_cast<int>(m_UnitStates[uid].hp) << std::endl;
            break;
        }
        case CommandType::Dead:
        {
            const GamePiece pieceType = cmd.die_s.whichPiece;

            auto it = m_pieces.find(pieceType);
            if (it == m_pieces.end()) break;

            auto& pieceInfo = it->second;

            m_manager->DestroyObject(pieceInfo.id);
            for (uint32_t subId : pieceInfo.subIds)
            {
                m_manager->DestroyObject(subId);
            }

            for (uint32_t effectId : pieceInfo.effectIds)
            {
                m_manager->DestroyObject(effectId);
            }
            pieceInfo.effectIds.clear();

            m_pieces.erase(it);

            // 해당 타일 상태 초기화
            int unitID = GetUnitID(pieceType);
            int tileID = m_UnitStates[unitID].targetTileID;
            m_tiles[tileID] = TileState{};

            // m_pieces가 전부 사라졌는지 체크
            if (m_pieces.size() == 0)
            {
                // 패킷 초기화
                yuno::net::packets::C2S_RoundStartReadyOK req{};

                // 패킷 바이너리화
                auto bytes = yuno::net::PacketBuilder::Build(
                    yuno::net::PacketType::C2S_RoundStartReadyOK,
                    [&](yuno::net::ByteWriter& w)
                    {
                        req.Serialize(w);
                    });

                // 패킷 보내기
                GameManager::Get().SendPacket(std::move(bytes));
            }
            break;
        }
        case CommandType::Turn_Over:
        {
            //cmd.over_s.whichPiece;
        }
        }
    }
}

void PlayGridSystem::CheckPacket(float dt)
{
    auto& mng = GameManager::Get();
    // 현재 씬이 '오토배틀'씬이 아니면 반환.
    if (mng.GetSceneState() != CurrentSceneState::AutoBattle)    return;



    // 게임 매니저에서 배틀 패킷 하나씩 받아옴
    if (!mng.IsEmptyBattlePacket() && !isProcessing)
    {
        const auto pckt = mng.PopBattlePacket();
        isProcessing = true;

        if (static_cast<int>(pckt.isCoinTossUsed) > 0 && m_delayFlag)
            m_pktTime = (pckt.actionTime  + 500 + 4500) / 1000.f; //임의값 넣어둠
        else if(m_delayFlag)
            m_pktTime = (pckt.actionTime + 500) / 1000.f;
        else if(static_cast<int>(pckt.isCoinTossUsed) > 0)
            m_pktTime = (pckt.actionTime + 4500) / 1000.f;
        else
            m_pktTime = (pckt.actionTime) / 1000.f;

        const auto runTimeCardID = pckt.runTimeCardID;
        const auto dir = pckt.dir;
        const std::vector<std::array<UnitState, 4>>& order = pckt.order;

        GamePiece whichPiece = GetGamePiece(pckt.pId, pckt.slotId);
        int mainUnit = GetUnitID(pckt.pId, pckt.slotId);

        // runtimeCardID로 CardManager에서 카드 정보 얻어옴
        // 서버 쪽에서 패킷 시간 계산 로직 끝나면 지워야함
        const auto& cardData = mng.GetCardData(runTimeCardID);

        const int effectID = cardData.m_effectId;
        const int soundID = cardData.m_soundId;

        const CardType cardType = cardData.m_type;
        
        if (cardType == CardType::Attack)    m_pktTime = attackPktTime;
        else if (cardType == CardType::Utility)    m_pktTime = utilityPktTime;
        //---------------------------------------------------
        ApplyActionOrder(order, mainUnit, runTimeCardID, (Direction)dir);

        // useID == 1 카드는 사용 즉시 손패에서 제거한다.
        if (pckt.pId == static_cast<uint8_t>(mng.GetPID()) && cardData.m_useId == 1)
        {
            mng.RemoveCard(runTimeCardID);
        }
    }

    // 장애물까지 발동하고 CheckOver
    if (!mng.IsEmptyObstaclePacket() && mng.IsEmptyBattlePacket() && !isProcessing)
    {
        m_delayFlag = false;
        const auto obstaclePkt = mng.PopObstaclePacket();
        if (isRoundOver) return;    // 라운드 종료 시 해당 장애물은 패킷 버리기
      
        m_pktTime = obstaclePktDuration;
        isProcessing = true;
        ApplyObstacleResult(obstaclePkt);
    }

    // 지금 처리 중인 패킷 시간 체크
    if (isProcessing)
    {
        m_currTime += dt;
        if (m_currTime >= m_pktTime)
        {
            isProcessing = false;
            m_currTime -= m_pktTime;
            std::cout << "Packet Time is Over\n";
            // ui weapon data에 현 유닛 상태 반영
            ReflectWeaponData();
            // 라운드 끝났는지 체크
            CheckOver();
        }
    }

}






void PlayGridSystem::UpdateSequence(float dt)
{
    UpdateAttackSequence(dt);
    UpdateUtilitySequence(dt);
    UpdateObstacleSequence(dt);
}

void PlayGridSystem::UpdateAttackSequence(float dt)
{
    if (!m_attackActive) return;
    
    auto& as = m_attackSequence;

    switch (as.attackPhase)
    {
    case AttackPhase::Alaram:
    {
        // alaram 시간 끝나고 다음 단계로 이동
        if (as.elapsed >= as.m_alarmDuration)
        {
            ++as.attackPhase;
            as.phaseStarted = true;
            as.elapsed = 0;
            break;
        }

        if (!as.phaseStarted) break;

        auto attackerIt = m_pieces.find(as.attacker);
        if (attackerIt == m_pieces.end())
        {
            as.attackPhase = AttackPhase::Over;
            break;
        }

        const auto& pieceInfo = attackerIt->second;
        auto pPiece = static_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));
        if (pPiece == nullptr)
        {
            as.attackPhase = AttackPhase::Over;
            break;
        }


        pPiece->SetDir(as.dir, true, attackRotDuration);
        //for (uint32_t subId : pieceInfo.subIds)
        //{
        //    auto pSubPiece = static_cast<UnitPiece*>(m_manager->FindObject(subId));
        //    if (pSubPiece != nullptr)
        //    {
        //        pSubPiece->SetDir(as.dir, true, attackRotDuration);
        //    }
        //}

        const auto& tiles = as.tileIDs;
        for (int i = 0; i < tiles.size(); i++)
        {
            int id = tiles[i];
            auto pTile = static_cast<UnitTile*>(m_manager->FindObject(m_tilesIDs[id]));
            pTile->SetFlashColor(as.m_alarmColor, as.m_flashCount, as.m_flashInterval);
        }
        as.phaseStarted = false;

        break;
    }
    case AttackPhase::Attack:
    {
        // unit attack 시간 끝나고 다음 단계로 이동
        if (as.elapsed >= as.m_attackDuration)
        {
            ++as.attackPhase;
            as.phaseStarted = true;
            as.elapsed = 0;
            break;
        }

        if (!as.phaseStarted) break;

        auto attackerIt = m_pieces.find(as.attacker);
        if (attackerIt == m_pieces.end())
        {
            as.attackPhase = AttackPhase::Over;
            break;
        }

        const auto& pieceInfo = attackerIt->second;
        auto pPiece = static_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));
        if (pPiece == nullptr)
        {
            as.attackPhase = AttackPhase::Over;
            break;
        }

        pPiece->InsertQ({ CommandType::Attack });

        //if(pieceInfo.subIds.size() == 0)    // 서브 유닛이 없는 경우에만
        //    pPiece->InsertQ({ CommandType::Attack });
        //else
        //    pPiece->InsertQ({ CommandType::Attack });
        //for (uint32_t subId : pieceInfo.subIds)
        //{
        //    auto pSubPiece = static_cast<UnitPiece*>(m_manager->FindObject(subId));
        //    if (pSubPiece != nullptr)
        //    {
        //        pSubPiece->InsertQ({ CommandType::Attack });
        //    }
        //}
        as.phaseStarted = false;
        
        std::cout << "[Attack Sequence]\nAttacker hp: " << static_cast<int>(m_UnitStates[GetUnitID(as.attacker)].hp) << std::endl;
        break;
    }
    case AttackPhase::Hit:
    {
        if (as.elapsed >= as.m_hitDuration)
        {
            ++as.attackPhase;
            as.phaseStarted = true;
            as.elapsed = 0;
            break;
        }

        if (!as.phaseStarted) break;

        // 타일 피격 이펙트 시작
        const auto& tiles = as.tileIDs;
        for (int i = 0; i < tiles.size(); i++)
        {
            int id = tiles[i];
            auto pTile = static_cast<UnitTile*>(m_manager->FindObject(m_tilesIDs[id]));

            Team team = m_pieces[as.attacker].team;

            Effect* eff = nullptr;;

            if((GameManager::Get().GetPID() == 1 && team == Team::Ally) || (GameManager::Get().GetPID() == 2 && team == Team::Enemy))
                eff = m_effectManager->Spawn(EffectID::Target, { 0, 0.01, 0 }, { 1, 1, 1 });
            else if((GameManager::Get().GetPID() == 1 && team == Team::Enemy) || (GameManager::Get().GetPID() == 2 && team == Team::Ally))
                eff = m_effectManager->Spawn(EffectID::TargetEnemy, { 0, 0.01, 0 }, { 1, 1, 1 });

            if(eff)
                pTile->Attach(eff);
            //pTile->SetFlashColor(as.m_tileEffectColor, as.m_flashCount, as.m_flashInterval);
        }

        AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_TileHit));

        // 기물 피격 애니메이션 시작
        const auto& pieces = as.hitPieces;
        for (int i = 0; i < pieces.size(); i++)
        {
            auto it = m_pieces.find(pieces[i]);
            if (it == m_pieces.end()) continue;

            const auto& pieceInfo = it->second;

            auto pPiece = static_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));
            if (pPiece == nullptr) continue;

            // 죽었는지 체크. 죽었으면 죽는 애니메이션 재생
            int unitID = GetUnitID(it->first);
            if (m_UnitStates[unitID].hp == 0)
            {
                pPiece->InsertQ(PlayGridQ::Dead_P(disappearDisolveDuration));
                for (auto& subId : pieceInfo.subIds)
                {
                    auto pSub = dynamic_cast<UnitPiece*>(m_manager->FindObject(subId));
                    pSub->InsertQ(PlayGridQ::Dead_P(disappearDisolveDuration));
                }
            }
            else // 살았다면 피격 애니메이션 재생
            {
                pPiece->InsertQ(PlayGridQ::Hit_P());
                for (auto& subId : pieceInfo.subIds)
                {
                    auto pSub = dynamic_cast<UnitPiece*>(m_manager->FindObject(subId));
                    pSub->InsertQ(PlayGridQ::Hit_P());
                }
            }
            std::cout << "[PlayGridSystem::UpdateAttackSequence]\nHitter hp: " << static_cast<int>(m_UnitStates[GetUnitID(pieces[i])].hp) << std::endl;
        }
        as.phaseStarted = false;
        break;
    }
    case AttackPhase::Over:
    {   
        as = {};
        m_attackActive = false;
        return;
    }
    }

    as.elapsed += dt;
}

void PlayGridSystem::UpdateUtilitySequence(float dt)
{
    if (!m_utilityActive) return;

    auto& us = m_utilitySequence;
    
    switch (us.utilityPhase)
    {
    case UtilityPhase::Move:
    {
        if (us.elapsed >= us.m_moveDuration)
        {
            ++us.utilityPhase;
            us.phaseStarted = true;
            us.elapsed = 0.f;
            break;
        }
        if (!us.phaseStarted)    break;

        const auto& pm = us.playerMove;

        ApplyMoveChanges(pm->dirty, pm->prevState, pm->snapshot, pm->mainUnit, pm->dir);

        us.phaseStarted = false;
        break;
    }
    case UtilityPhase::AttackAndMove:
    {
        if (us.elapsed >= us.m_attackAndMoveDuration)
        {
            ++us.utilityPhase;
            us.phaseStarted = true;
            us.elapsed = 0.f;
            break;
        }

        if (!us.phaseStarted)    break;

        auto& as = m_attackSequence;
        bool condition1 = as.attackPhase == AttackPhase::Hit;
        bool condition2 = as.phaseStarted == false;
        bool condition3 = us.hitMove != HitMove::None;      // 넉백 또는 그랩이 있는가

        if (!(condition1 && condition2 && condition3))  break;

        const auto& pieces = as.hitPieces;
        const auto& hm = us.hittersMove;
        if (pieces.size() != hm.size())
        {
            std::cout << "hitter count and hitter move ain't same!\n";
            assert(false);
        }
        for (int i = 0; i < pieces.size(); i++)
        {
            if (!CheckNotDying(pieces[i]))    continue;
            ApplyMoveChanges(hm[i]->dirty, hm[i]->prevState, hm[i]->snapshot, hm[i]->mainUnit, hm[i]->dir);
        }

        us.phaseStarted = false;

        break;
    }
    case UtilityPhase::Buff:
    {
        if (us.elapsed >= us.m_buffDuration)
        {
            ++us.utilityPhase;
            us.phaseStarted = true;
            us.elapsed = 0.f;
            break;
        }

        if (!us.phaseStarted)    break;

        if(us.buffData != nullptr)
            BuffEvent(us.playPiece, us.buffData);

        break;
    }
    case UtilityPhase::Over:
    {
        // 초기화
        m_utilityActive = false;
        delete us.playerMove;
        for (auto& hm : us.hittersMove)
        {
            delete hm;
        }
        us.hittersMove.clear();
        m_utilitySequence = {};
        return;
    }
    }

    us.elapsed += dt;
}

void PlayGridSystem::UpdateObstacleSequence(float dt)
{
    if (!m_obstacleActive)   return;
    
    auto& os = m_obstacleSequence;

    switch (os.obstaclePhase)
    {
    case ObstaclePhase::Trigger:
    {
        if (os.elapsed >= os.m_triggerDuration || os.hitTileIDs.size() == 0)
        {
            os.obstaclePhase = ObstaclePhase::Warning;
            os.phaseStarted = true;
            os.elapsed = 0;
            break;
        }
        if (!os.phaseStarted)    break;
        
        // 경고 이펙트 제거
        for (const auto& tileID : os.hitTileIDs)
        {
            if (m_tiles[tileID].effectIDs.size() == 0) continue;
            for (auto effectID : m_tiles[tileID].effectIDs)
            {
                m_manager->DestroyObject(effectID);
            }
            m_tiles[tileID].effectIDs.clear();
        }

        for (const auto& tileID : os.hitTileIDs)
        {
            auto pTile = dynamic_cast<UnitTile*>(m_manager->FindObject(m_tilesIDs[tileID]));
            pTile->PlayTrigger(os.attackType, os.hitColor, os.hitFlashCount, os.hitFlashInterval);
            auto [cx, cz] = GetCellByID(tileID);
            if(os.attackType == ObstacleType::Collapse)
                ChangeTileTO(cx, cz, TileOccupy{ TileOccuType::Collapesed, TileWho::None });
        }

        // 기물
        if (os.hitPieces.size() != 0)
        {
            for (auto& piece : os.hitPieces)
            {
                auto pieceIt = m_pieces.find(piece);
                if (pieceIt == m_pieces.end())   continue;
                auto& pieceInfo = pieceIt->second;

                int unitID = GetUnitID(piece);
                auto pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));
                if (m_UnitStates[unitID].hp == 0)
                {
                    pPiece->InsertQ(PlayGridQ::Dead_P(disappearDisolveDuration));
                    for (auto& subId : pieceInfo.subIds)
                    {
                        auto pSub = dynamic_cast<UnitPiece*>(m_manager->FindObject(subId));
                        pSub->InsertQ(PlayGridQ::Dead_P(disappearDisolveDuration));
                    }
                }
                else
                {
                    pPiece->InsertQ(PlayGridQ::Hit_P());
                    for (auto& subId : pieceInfo.subIds)
                    {
                        auto pSub = dynamic_cast<UnitPiece*>(m_manager->FindObject(subId));
                        pSub->InsertQ(PlayGridQ::Hit_P());
                    }
                }
            }
        }

        std::cout << "[PlayGridSystem] Obstacle is triggered\n";

        os.phaseStarted = false;

        break;
    }
    case ObstaclePhase::Warning:
    {
        if (os.elapsed >= os.m_warningDuration || os.warningTileIDs.size() == 0)
        {
            os.obstaclePhase = ObstaclePhase::Over;
            os.elapsed = 0;
            break;
        }
        if (!os.phaseStarted)    break;

        // 타일
        for (const auto& tileID : os.warningTileIDs)
        {
            auto pTile = dynamic_cast<UnitTile*>(m_manager->FindObject(m_tilesIDs[tileID]));
            pTile->PlayWarning(os.attackType, os.warnColor, os.warnFlashCount, os.warnFlashInterval);

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
            ed.rot = { XMConvertToRadians(90.f), 0, 0 };
            ed.isLoop = true;
            ed.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_1.png";
            if (os.attackType != ObstacleType::Collapse)
            {
                m_effectManager->RegisterEffect(ed);
                auto pEffect1 = m_manager->CreateObject<EffectUnit>(L"BarrierWarning1", XMFLOAT3(0, 0.01f, 0));
                pEffect1->BuildInternalEffectMaterial(ed);
                pTile->Attach(pEffect1);
                m_tiles[tileID].effectIDs.push_back(pEffect1->GetID());
            }

            ed.id = EffectID::FloorWarning2;
            ed.framecount = 30;
            ed.lifetime = 0.4f;
            ed.cols = 5;
            ed.rows = 6;
            ed.rot = { -XMConvertToRadians(90.f), 0, 0};
            ed.texPath = L"../Assets/Effects/Warning/EF_Floor_WARNING_2.png";
            m_effectManager->RegisterEffect(ed);
            auto pEffect2 = m_manager->CreateObject<EffectUnit>(L"BarrierWarning2", XMFLOAT3(0, 0.01f, 0));
            pEffect2->BuildInternalEffectMaterial(ed);
            pTile->Attach(pEffect2);
            m_tiles[tileID].effectIDs.push_back(pEffect2->GetID());
        }

        std::cout << "[PlayGridSystem] Warning Next Obstacle\n";

        os.phaseStarted = false;

        break;
    }
    case ObstaclePhase::Over:
    {
        os = {};
        m_obstacleActive = false;
        return;
    }
    }

    os.elapsed += dt;
}

void PlayGridSystem::ApplyActionOrder(const std::vector<std::array<UnitState, 4>>& order, int mainUnit, uint32_t runCardID, Direction dir)
{
    auto& mng = GameManager::Get();
    // 카드 데이터들
    const auto& cardData = mng.GetCardData(runCardID);                  // 카드 일반 데이터
    const RangeData* rangeData = mng.GetRangeData(runCardID);           // 카드 (공격)범위 데이터
    const auto& ranges = rangeData->offsets;
    const CardEffectData* buffData = mng.GetCardEffectData(runCardID);  // 카드 버프 데이터

    CardType cardType = cardData.m_type;
    // 행동 순서
    for (int i = 0; i < order.size(); i++)
    {
        const std::array<UnitState, 4>& unitStates_Now = order[i];

        Dirty_US dirty = Diff_US(m_UnitStates[mainUnit], unitStates_Now[mainUnit]);
        
        switch (cardType)
        {
        case CardType::Buff:
            ApplyBuffChanges(mainUnit, buffData);
            break;
        case CardType::Move: 
            ApplyMoveChanges(dirty, unitStates_Now, mainUnit, dir);
            break;
        case CardType::Attack:
            ApplyAttackChanges(dirty, unitStates_Now, mainUnit, ranges, dir);
            break;
        case CardType::Utility:
            ApplyUtilityChanges(dirty, unitStates_Now, mainUnit, ranges, dir, buffData, i);
            break;
        }

        m_UnitStates = unitStates_Now;
    }
}

bool PlayGridSystem::ApplyBuffChanges(int mainUnit, const CardEffectData*& buffData)
{
    const UnitState US = m_UnitStates[mainUnit];
    GamePiece whichPiece = GetGamePiece(US.pId, US.slotId);
    if (!CheckNotDying(whichPiece))
    {
        // 부활 규칙이 없으므로 return 합니다.
        m_pktTime -= buffDuration;
        return false;
    }

    if (!BuffEvent(whichPiece, buffData))
        return false;

    return true;
}

bool PlayGridSystem::ApplyMoveChanges
(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, Direction dir)
{
    return ApplyMoveChanges(dirty, m_UnitStates[mainUnit], newUnitStates, mainUnit, dir);

    //const UnitState prevUS = m_UnitStates[mainUnit];
    //const UnitState newUS = newUnitStates[mainUnit];
    //GamePiece whichPiece = GetGamePiece(newUS.pId, newUS.slotId);
    //if (!CheckNotDying(whichPiece))
    //{
    //    // 부활 규칙이 없으므로 return 합니다.
    //    m_pktTime -= moveDuration;
    //    return false;
    //}

    //auto newcell = GetCellByID(newUS.targetTileID);
    //auto oldcell = GetCellByID(prevUS.targetTileID);

    //// 이동하다가 충돌했을 때
    //bool isCollided = static_cast<bool>(newUS.isEvent);
    //if (isCollided)
    //{
    //    if (HasThis_US(dirty, Dirty_US::hp))
    //    {
    //        MoveEvent(whichPiece, oldcell, newcell, dir, true, true);
    //    }
    //    else
    //    {
    //        MoveEvent(whichPiece, oldcell, newcell, dir, true);
    //    }
    //}
    //// 이동만 할 때
    //else if (HasThis_US(dirty, Dirty_US::targetTileID))
    //{
    //    MoveEvent(whichPiece, oldcell, newcell, dir);
    //}
    //else    // 충돌 X & 이동 X 일 경우
    //{
    //    m_pktTime -= moveDuration;
    //    return false;
    //}
    //return true;
}

bool PlayGridSystem::ApplyMoveChanges(Dirty_US dirty, const UnitState& prevUS, const std::array<UnitState, 4>& newUnitStates, int mainUnit, Direction dir)
{
    const UnitState newUS = newUnitStates[mainUnit];
    GamePiece whichPiece = GetGamePiece(newUS.pId, newUS.slotId);
    if (!CheckNotDying(whichPiece))
    {
        // 부활 규칙이 없으므로 return 합니다.
        m_pktTime -= moveDuration;
        return false;
    }

    auto newcell = GetCellByID(newUS.targetTileID);
    auto oldcell = GetCellByID(prevUS.targetTileID);

    // 이동하다가 충돌했을 때
    bool isCollided = static_cast<bool>(newUS.isEvent);
    if (isCollided)
    {
        if (HasThis_US(dirty, Dirty_US::hp))
        {
            MoveEvent(whichPiece, oldcell, newcell, dir, true, true);
        }
        else
        {
            MoveEvent(whichPiece, oldcell, newcell, dir, true);
        }
    }
    // 이동만 할 때
    else if (HasThis_US(dirty, Dirty_US::targetTileID))
    {
        MoveEvent(whichPiece, oldcell, newcell, dir);
    }
    else    // 충돌 X & 이동 X 일 경우
    {
        m_pktTime -= moveDuration;
        return false;
    }
    return true;
}

bool PlayGridSystem::ApplyAttackChanges
(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, const std::vector<RangeOffset>& ranges, Direction dir)
{
    const UnitState prevUS = m_UnitStates[mainUnit];
    const UnitState newUS = newUnitStates[mainUnit];
    GamePiece whichPiece = GetGamePiece(newUS.pId, newUS.slotId);
    if (!CheckNotDying(whichPiece))
    {
        // 부활 규칙이 없으므로 return 합니다.
        m_pktTime -= attackAndMoveDuration;
        return false;
    }
    Int2 cellPos = { GetCellByID(prevUS.targetTileID) };
    const auto targetTileIDs = GetRangeTileIDs(cellPos, ranges, dir);

    // 타격할 타일이 없으므로 return 합니다.
    if (targetTileIDs.size() == 0)
    {
        m_pktTime -= attackAndMoveDuration;
        return false;
    }

    // 공격 시퀀스 채워넣기
    auto& as = m_attackSequence;
    as.attacker = whichPiece;
    as.dir = dir;
    as.tileIDs = targetTileIDs;
    as.attackPhase = AttackPhase::Alaram;
    as.phaseStarted = m_attackActive = true;

    // 공격 팀 확인. 피격 팀 확인하기.
    Team attackTeam = m_pieces[whichPiece].team;
    //Float4 allyColor = Float4{ 0, 0, 0.8f, 1 };
    Float4 allyColor = Float4{ 0.6f, 0.6f, 1.0f, 1 };
    //Float4 enemyColor = Float4{ 0.8f, 0, 0, 1 };
    Float4 enemyColor = Float4{ 1.0f, 0.6f, 0.6f, 1 };
    as.m_alarmColor = (attackTeam == Team::Ally) ? allyColor : enemyColor;

    for (int i = 0; i < targetTileIDs.size(); i++)
    {
        auto& tileTO = m_tiles[targetTileIDs[i]].to;
        TileOccuType targetTeam = tileTO.occuType;

        bool condition1 = attackTeam == Team::Ally && targetTeam == TileOccuType::Enemy_Occupied;
        bool condition2 = attackTeam == Team::Enemy && targetTeam == TileOccuType::Ally_Occupied;

        if (condition1 || condition2)
        {
            as.hitPieces.push_back((GamePiece)tileTO.who);
        }
    }

    as.m_flashCount = tileFlashCount;
    as.m_flashInterval = tileFlashInterval;
    as.m_alarmDuration = tileFlashDuration;
    as.m_hitDuration = hitDuration;
    as.m_attackDuration = attackDuration;

    return true;
}

bool PlayGridSystem::ApplyUtilityChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit,
    const std::vector<RangeOffset>& ranges, Direction dir, const CardEffectData*& buffData, int snapNum)
{
    const UnitState prevUS = m_UnitStates[mainUnit];
    const UnitState newUS = newUnitStates[mainUnit];
    GamePiece whichPiece = GetGamePiece(newUS.pId, newUS.slotId);
    if (!CheckNotDying(whichPiece))
    {
        // 부활 규칙이 없으므로 return 합니다.
        return false;
    }

    auto& us = m_utilitySequence;

    m_utilityActive = us.phaseStarted = true;
    us.utilityPhase = UtilityPhase::Move;
    us.playPiece = whichPiece;

    switch (snapNum)
    {
        // 1. 이동 값
        case 0:
        {
            if (!HasThis_US(dirty, Dirty_US::targetTileID))
                break;

            us.playerMove = new MoveInfo{ dirty, m_UnitStates[mainUnit], newUnitStates, mainUnit, dir };
            us.m_moveDuration = moveDuration;
            break;
        }
        // 2. 공격 값
        case 1:
        {
            if (!ApplyAttackChanges(dirty, newUnitStates, mainUnit, ranges, dir))
                break;

            us.m_attackAndMoveDuration = attackAndMoveDuration;
            break;
        }
        // 3. 그랩/넉백 & 버프 값
        case 2:
        {
            float buffDuration = 0.5f;

            const auto& hps = m_attackSequence.hitPieces;
            if (hps.size() == 0)
                break;
            for (int i = 0; i < hps.size(); i++)
            {
                int unitID = GetUnitID(hps[i]);
                Dirty_US d = Diff_US(m_UnitStates[unitID], newUnitStates[unitID]);
                MoveInfo* mi = new MoveInfo{ d, m_UnitStates[unitID], newUnitStates, unitID, dir };
                us.hittersMove.push_back(mi);
                if (HasThis_US(d, Dirty_US::targetTileID))   us.hitMove = HitMove::Move;
            }
            us.buffData = buffData;
            us.m_buffDuration = buffDuration;
            break;
        }
    }
    return true;
}

const std::vector<int> PlayGridSystem::GetRangeTileIDs(const Int2 unitCell, const std::vector<RangeOffset>& ranges, Direction dir)
{
    std::vector<int> tileIDs;
    // 그리드가 여러 개라면, SetNG_P() 호출해줘야 함

    std::vector<Int2> relativeRanges;
    for (int i = 0; i < ranges.size(); i++)
    {
        const auto& range = ranges[i];

        switch (dir)
        {
        case Direction::Up:
            relativeRanges.push_back({ -range.dy, -range.dx });
            break;
        case Direction::Down:
            relativeRanges.push_back({ -range.dy, range.dx });
            break;
        case Direction::Left:
            relativeRanges.push_back({ -range.dx, range.dy });
            break;
        case Direction::Right:
            relativeRanges.push_back({ range.dx, range.dy });
            break;
        }
    }
    std::vector<Int2> absoluteRanges;

    for (int i = 0; i < relativeRanges.size(); i++)
    {
        int cx = unitCell.x + relativeRanges[i].x;
        int cz = unitCell.y + relativeRanges[i].y;
        if(!(m_grids[(int)m_nowG]->InBounds(cx, cz)))   continue;
        absoluteRanges.push_back({ cx, cz });
    }

    for (int i = 0; i < absoluteRanges.size(); i++)
    {
        tileIDs.push_back(GetID(absoluteRanges[i].x, absoluteRanges[i].y));
    }

    return tileIDs;
}

void PlayGridSystem::MoveEvent(const GamePiece& pieceType, Int2 oldcell, Int2 newcell, Direction moveDir,
    bool isCollided, bool isEnemy)
{
    // 죽어가고 있는 기물인지 확인
    if (!CheckNotDying(pieceType)) return;

    // 이동 후, 상태 수정을 위해 참조로 받음
    auto pieceIt = m_pieces.find(pieceType);
    if (pieceIt == m_pieces.end())   return;
    PieceInfo& pieceInfo = pieceIt->second;

    // 아이디로 오브젝트 포인터 받아오기
    UnitPiece* pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));
    if (pPiece == nullptr) return;

    // 기존 타일과 이동할 타일의 상태
    const TileOccupy oldTo = GetTileTO(oldcell.x, oldcell.y);
    const TileOccupy to = GetTileTO(newcell.x, newcell.y);

    // 기물의 상태 회전 방향 체크 및 변경
    auto dir = Get2Dir(oldcell.x, oldcell.y, newcell.x, newcell.y);
    auto fdir = pieceInfo.dir = (dir == Direction::Same) ? pieceInfo.dir : dir;

    // 타 기물과 충돌했을 경우 충돌지점
    //auto moveDir = Get8Dir(oldcell.x, oldcell.y, newcell.x, newcell.y);             // 진입 방향
    auto [colW, colC] = GetCollisionPos(moveDir, newcell.x, newcell.y);

    auto [wx, wz] = m_grids[(int)m_nowG]->CellToWorld(newcell.x, newcell.y);

    TileWho who = static_cast<TileWho>(pieceType);

    if (to.occuType == TileOccuType::Collapesed)       // 없어진 자리
    {
        std::cout << "[PlayGridSystem]::Collapesed\n";

        return;
    }
    else if (isCollided)                                    // 충돌 시
    {
        if (isEnemy)                                        // 적군과 충돌
        {
            std::cout << "[PlayGridSystem]::Enemy_Collison\n";

            CommandType cmd = CommandType::Hit;
            bool amIdead = (m_UnitStates[GetUnitID(pieceType)].hp == 0) ? true : false;
            // 부딪힌 기물 타입 확인
            GamePiece existWho = GamePiece::None;
            bool isIn = m_grids[(int)m_nowG]->InBounds(colC.x, colC.y);
            if (isIn)
            {
                int tileID = GetID(colC.x, colC.y);
                existWho = (GamePiece)m_tiles[tileID].to.who;
            }

            // 충돌지점까지 이동 후 원래 자리로 되돌아감
            pPiece->InsertQ(PlayGridQ::Rot_P(fdir));
            pPiece->InsertQ(PlayGridQ::Move_P(colW.x, m_wy, colW.y));       // 충돌 위치까지 이동 후
            pPiece->InsertQ(PlayGridQ::MoveHit_P(existWho, amIdead, disappearDisolveDuration));                                 // 이동하는 애 죽었는지 부딪힌 애 죽었는지
            pPiece->InsertQ(PlayGridQ::Move_P(wx, m_wy, wz, 1));            // 제자리로 돌아감
        }
        else                                                // 아군과 충돌
        {
            std::cout << "[PlayGridSystem]::Ally_Collison\n";

            // 충돌지점까지 이동 후 원래 자리로 되돌아감
            pPiece->InsertQ(PlayGridQ::Rot_P(fdir));
            pPiece->InsertQ(PlayGridQ::Move_P(colW.x, m_wy, colW.y));
            pPiece->InsertQ(PlayGridQ::Move_P(wx, m_wy, wz, 1));
        }
    }
    else                                                                                // 비어있는 자리
    {
        std::cout << "[PlayGridSystem]::Unoccupied\n";
        
        // 기물 이동
        pPiece->InsertQ(PlayGridQ::Rot_P(fdir));
        pPiece->InsertQ(PlayGridQ::Move_P(wx, m_wy, wz, 1));
    }
    
    // 타일 상태 변경
    ChangeTileTO(oldcell.x, oldcell.y, TileOccupy{ TileOccuType::Unoccupied, TileWho::None });
    ChangeTileTO(newcell.x, newcell.y,
        (pieceInfo.team == Team::Ally) ?
        TileOccupy{ TileOccuType::Ally_Occupied, who } :
        TileOccupy{ TileOccuType::Enemy_Occupied, who });
}

bool PlayGridSystem::BuffEvent(const GamePiece& pieceType, const CardEffectData*& buffData)
{
    auto it = m_pieces.find(pieceType);
    if (it == m_pieces.end())
    {
        m_pktTime -= buffDuration;
        return false;
    }

    const auto& pieceInfo = it->second;

    auto applyFlash = [&](Float4 color, EffectID effectID)
        {
            auto pPiece = static_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));
            if (pPiece != nullptr && pieceInfo.subIds.size() == 0)
            {
                //pPiece->SetFlashColor(color, 1, buffDuration);
                auto eff = m_effectManager->Spawn(effectID, { 0.1f, 0.4f, 0 }, { 1.f, 1.f, 1.f });
                pPiece->Attach(eff);
            }
                

            for (uint32_t subId : pieceInfo.subIds)
            {
                auto pSubPiece = static_cast<UnitPiece*>(m_manager->FindObject(subId));
                if (pSubPiece != nullptr)
                {
                    //pSubPiece->SetFlashColor(color, 1, buffDuration);
                    auto eff = m_effectManager->Spawn(effectID, { 0.1f, 0.4f, 0 }, { 1.f, 1.f, 1.f });
                    pSubPiece->Attach(eff);
                }
            }
        };

    const auto& data = *buffData;

    if (data.m_giveDamageBonus != 0)            // 공격력 증가(버프)
    {
        Float4 red = { 1, 0, 0, 1 };
        applyFlash(red, EffectID::PowerUpBuff);
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_PowerUpBuff));
    }
    else if (data.m_takeDamageReduce != 0)      // 방어력 증가(버프)
    {
        Float4 yellow = { 1, 1, 0, 1 };
        applyFlash(yellow, EffectID::ShieldBuff);
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_ShieldBuff));
    }
    else if (data.m_takeDamageIncrease != 0)    // 방어력 저하(디버프)
    {
        Float4 blue = { 0, 0.f, 1, 1 };
        applyFlash(blue, EffectID::DeBuff);
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_DeBuff));
    }
    else if (data.m_staminaRecover != 0)        // 스태미나 회복
    {
        Float4 green = { 0, 1, 0, 1 };
        applyFlash(green, EffectID::StaminaBuff);
        AudioQ::Insert(AudioQ::PlayOneShot(EventName::PLAYER_StaminaBuff));
    }

    return true;
}

void PlayGridSystem::CheckOver()
{
    if (isRoundOver) return;

    bool deadUnitsByPlayer[4]{}; // [P1-S1, P1-S2, P2-S1, P2-S2]
    for (int i = 0; i < 4; i++)
    {
        deadUnitsByPlayer[i] = (m_UnitStates[i].hp == 0);
    }

    const bool myIsP1 = (m_pID == 1);

    bool deadUnits[4]{}; // [Ally1, Ally2, Enemy1, Enemy2] (클라 로컬 관점)
    deadUnits[(int)GamePiece::Ally1] = myIsP1 ? deadUnitsByPlayer[0] : deadUnitsByPlayer[2];
    deadUnits[(int)GamePiece::Ally2] = myIsP1 ? deadUnitsByPlayer[1] : deadUnitsByPlayer[3];
    deadUnits[(int)GamePiece::Enemy1] = myIsP1 ? deadUnitsByPlayer[2] : deadUnitsByPlayer[0];
    deadUnits[(int)GamePiece::Enemy2] = myIsP1 ? deadUnitsByPlayer[3] : deadUnitsByPlayer[1];

    const bool allyDead = deadUnits[(int)GamePiece::Ally1] && deadUnits[(int)GamePiece::Ally2];
    const bool enemyDead = deadUnits[(int)GamePiece::Enemy1] && deadUnits[(int)GamePiece::Enemy2];
    
    RoundResult roundResult = RoundResult::None;
    if (allyDead && enemyDead)          // 무승부
    {
        if(GameManager::Get().GetEndGame())
            GameManager::Get().SetSceneState(CurrentSceneState::ResultScene);
        std::cout << "This Round Result : Draw\n";
        roundResult = RoundResult::Draw;
    }
    else if (allyDead)                  // Lose
    {
        if (GameManager::Get().GetEndGame())
            GameManager::Get().SetSceneState(CurrentSceneState::ResultScene);
        std::cout << "This Round Result : Lose\n";
        roundResult = (myIsP1) ? RoundResult::Winner_P2 : RoundResult::Winner_P1;
    }
    else if (enemyDead)                 // Win
    {
        if (GameManager::Get().GetEndGame())
            GameManager::Get().SetSceneState(CurrentSceneState::ResultScene);
        std::cout << "This Round Result : Win\n";
        roundResult = (myIsP1) ? RoundResult::Winner_P1 : RoundResult::Winner_P2;
    }
    else
        return;

    for (GamePiece i = GamePiece::Ally1; i < GamePiece::MAX; ++i)
    {
        if (deadUnits[(int)i]) continue;

        auto it = m_pieces.find(i);
        if (it == m_pieces.end()) continue;

        auto& pieceinfo = it->second;
        auto pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceinfo.id));
        if (pPiece != nullptr)
        {
            // 라운드 종료 시 남아있는 기물도 일괄 제거
            pPiece->InsertQ(PlayGridQ::Disappear_P(disappearDisolveDuration));
        }

        for (auto sid : pieceinfo.subIds)
        {
            auto pSub = dynamic_cast<UnitPiece*>(m_manager->FindObject(sid));
            if (pSub != nullptr)
                pSub->InsertQ(PlayGridQ::Disappear_P(disappearDisolveDuration));
        }
    }
    isRoundOver = true;
    GameManager::Get().ClearBattlePacket();
    GameManager::Get().SetRoundResult(roundResult);
}

void PlayGridSystem::ReflectWeaponData()
{
    auto& mng = GameManager::Get();

    std::array<Wdata, 4> datas;
    for (int i = 0; i < m_UnitStates.size(); i++)
    {
        datas[i].pId = m_UnitStates[i].pId;
        datas[i].slotId = m_UnitStates[i].slotId;
        datas[i].weaponId = m_weaponIDs[i];
        datas[i].hp = m_UnitStates[i].hp;
        datas[i].stamina = m_UnitStates[i].stamina;
        datas[i].currentTile = m_UnitStates[i].targetTileID;
    }
    mng.SetUIWeaponData(datas);
}

void PlayGridSystem::ApplyObstacleResult(const ObstacleResult& obstacle)
{
    std::cout
        << "[PlayGridSystem] Obstacle packet received"
        << " triggerSnapshot(PrevRound) + warning(NextRound)"
        << " obstacleID=" << int(obstacle.obstacleID)
        << " tiles=";

    for (const auto& tile : obstacle.tileIDs) 
    {
        std::cout << static_cast<int>(tile) << " ";
    }
    std::cout << std::endl;

    bool hasTriggerSnapshot = false;
    Float4 warnColor{ 1.0f, 0.0f, 0.0f, 1.f };


    // ObstacleSequence 정보 기입
    auto& os = m_obstacleSequence;
    m_obstacleActive = true;
    os.phaseStarted = true;
    os.obstaclePhase = ObstaclePhase::Trigger;

    for (int i = 0; i < static_cast<int>(m_UnitStates.size()); ++i)
    {
        const auto& prev = m_UnitStates[i];
        const auto& cur = obstacle.unitState[i];

        if (cur.pId == 0 || cur.slotId == 0)
            continue;


        std::cout
            << "  [Trigger] unit[" << i << "]"
            << " hp " << int(prev.hp) << "->" << int(cur.hp)
            << " tile " << int(prev.targetTileID) << "->" << int(cur.targetTileID)
            << "\n";


        int unitID = GetUnitID(cur.pId, cur.slotId);
        if (m_UnitStates[unitID].hp == 0)
            continue;

        GamePiece piece = GetGamePiece(cur.pId, cur.slotId);
        
        if (prev.hp != cur.hp)
        {
            os.hitPieces.push_back(piece);
            hasTriggerSnapshot = true;
        }


        //// 장애물 발동해서 피 깎였으면 여기서 색 바꿔줌
        //if (prev.hp != cur.hp)
        //{
        //    auto* pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceIt->second.id));
        //    if (pPiece)
        //        pPiece->SetFlashColor(warnColor, 5, tileFlashInterval*3.3f);
        //
        //    for (uint32_t subId : pieceIt->second.subIds)
        //    {
        //        auto* pSubPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(subId));
        //        if (pSubPiece)
        //            pSubPiece->SetFlashColor(warnColor, 5, tileFlashInterval*3.3f);
        //    }
        //}

        //if (prev.hp > cur.hp)
        //    CheckHealth(cur, pieceIt->second);
    }

    // ObstacleSequence 장애물 발동 정보 기입
    if (m_obstacleTile.tileIDs.size() == 0)
    {
        os.obstaclePhase = ObstaclePhase::Warning;
        m_pktTime -= triggerDuration;
    }
    else
    {
        os.hitColor = warnColor;
        os.hitFlashCount = hitFlashCount;
        os.hitFlashInterval = hitFlashInterval;
        os.m_triggerDuration = triggerDuration;
        os.attackType = m_obstacleTile.obstacleID;
        os.hitTileIDs = m_obstacleTile.tileIDs;
    }

    if (hasTriggerSnapshot) // 누군가 맞은 애 있으면 obstacle 패킷에서 받은 스냅샷으로 현재 유닛 상태 변경
    {
        m_UnitStates = obstacle.unitState;
    }


    // 다음 장애물 경고    // 이펙트 넣기★
    if (obstacle.obstacleID == ObstacleType::Horizon_Razer || obstacle.obstacleID == ObstacleType::Vertical_Razer) 
        warnColor = { 0.0f, 1.0f, 0.0f, 1.f };
    else if (obstacle.obstacleID == ObstacleType::Collapse) 
        warnColor = { 0.0f, 0.0f, 1.0f, 1.f };

    //for (auto tileId : obstacle.tileIDs)
    //{
    //    if (tileId == 0 || tileId >= m_tilesIDs.size())
    //        continue;

    //    auto* pTile = static_cast<UnitTile*>(m_manager->FindObject(m_tilesIDs[tileId]));
    //    if (pTile)
    //        pTile->SetFlashColor(warnColor, 5, tileFlashInterval*3.3f);
    //}
    // ObstacleSequence 경고 정보 기입
    os.warnColor = warnColor;
    os.warnFlashCount = warnFlashCount;
    os.warnFlashInterval = warnFlashInterval;
    os.m_warningDuration = warnDuration;
    os.warningType = obstacle.obstacleID;
    os.warningTileIDs = obstacle.tileIDs;

    // 장애물 정보 갱신
    m_obstacleTile.obstacleID = obstacle.obstacleID;
    m_obstacleTile.tileIDs = obstacle.tileIDs;
}

bool PlayGridSystem::CheckNotDying(const GamePiece pieceType)
{
    auto pIt = m_pieces.find(pieceType);
    if (pIt == m_pieces.end())
    {
        std::cout << "[PlayGridSystem]::It's Already Dead!!\n";
        return false;
    }
    // 죽는 중인지까지 체크
    int unitId = GetUnitID(pieceType);
    if (m_UnitStates[unitId].hp == 0)
    {
        return false;
    }
    return true;
}

void PlayGridSystem::ChangeTileTO(int cx, int cz, const TileOccupy to)
{
    m_tiles[GetID(cx, cz)].to = to;
}

const TileOccupy PlayGridSystem::GetTileTO(int cx, int cz)
{
    return m_tiles[GetID(cx, cz)].to;
}

Direction PlayGridSystem::Get2Dir(float oldcx, float oldcz, float cx, float cz)
{
    int dx = cx - oldcx;

    if (dx > 0)  return Direction::Right;
    if (dx < 0)  return Direction::Left;
    return Direction::Same;
}

Direction PlayGridSystem::Get8Dir(float oldcx, float oldcz, float cx, float cz)
{
    int dx = (cx > oldcx) - (cx < oldcx);   // -1, 0, 1
    int dz = (cz > oldcz) - (cz < oldcz);   // -1, 0, 1

    if (dx == 0) return (dz < 0) ? Direction::Up : Direction::Down;
    if (dz == 0) return (dx > 0) ? Direction::Right : Direction::Left;

    // 대각선
    if (dx > 0) return (dz < 0) ? Direction::UpRight : Direction::DownRight;
    else        return (dz < 0) ? Direction::UpLeft : Direction::DownLeft;

}

std::pair<Float2, Int2> PlayGridSystem::GetCollisionPos(Direction dir, int cx, int cz)
{
    auto [wx, wz] = m_grids[(int)m_nowG]->CellToWorld(cx, cz);
    float halfwidth = (m_cellSizeX) / 2.f;
    float halfheight = (m_cellSizeZ) / 2.f;
    float width = (m_cellSizeX);
    float height = (m_cellSizeZ);

    Float2 newW;
    Int2 newC;

    //int inverse = (pieceDir == Direction::Right) ? 1 : -1;

    switch (dir)
    {
    case Direction::Up:
        newW.x = wx;
        newW.y = wz + height;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx;
        newW.y = wz + halfheight;
        break;
    case Direction::Down:
        newW.x = wx;
        newW.y = wz - height;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx;
        newW.y = wz - halfheight;
        break;
    case Direction::Right:
        newW.x = wx + (width);
        newW.y = wz;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx + (halfwidth);
        newW.y = wz;
        break;
    case Direction::Left:
        newW.x = wx - (width);
        newW.y = wz;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx - (halfwidth);
        newW.y = wz;
        break;
    case Direction::UpLeft:
        newW.x = wx - (width);
        newW.y = wz + height;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx - (halfwidth);
        newW.y = wz + halfheight;
        break;
    case Direction::UpRight:
        newW.x = wx + (width);
        newW.y = wz + height;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx + (halfwidth);
        newW.y = wz + halfheight;
        break;
    case Direction::DownLeft:
        newW.x = wx - (width);
        newW.y = wz - height;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx - (halfwidth);
        newW.y = wz - halfheight;
        break;
    case Direction::DownRight:
        newW.x = wx + (width);
        newW.y = wz - height;
        newC = m_grids[(int)m_nowG]->WorldToCell(newW.x, newW.y);
        newW.x = wx + (halfwidth);
        newW.y = wz - halfheight;
        break;
    }

    return { newW, newC };
}

GamePiece PlayGridSystem::GetGamePiece(int pId, int unitId)
{
    bool isMyTurn = (pId == m_pID) ? true : false;
    GamePiece whichPiece = (isMyTurn) ?
        (unitId == 1) ? GamePiece::Ally1 : GamePiece::Ally2 :
        (unitId == 1) ? GamePiece::Enemy1 : GamePiece::Enemy2;
    return whichPiece;
}

int PlayGridSystem::GetUnitID(int pId, int slotID)
{
    int unitID = (pId == 1) ?
        (slotID == 1) ? 0 : 1 :
        (slotID == 1) ? 2 : 3;
    return unitID;
}

int PlayGridSystem::GetUnitID(GamePiece gamePiece)
{
    if (m_pID == 1)
    {
        return
            (gamePiece == GamePiece::Ally1) ? 0 :
            (gamePiece == GamePiece::Ally2) ? 1 :
            (gamePiece == GamePiece::Enemy1) ? 2 : 3;
    }
    else
    {
        return
            (gamePiece == GamePiece::Ally1) ? 2 :
            (gamePiece == GamePiece::Ally2) ? 3 :
            (gamePiece == GamePiece::Enemy1) ? 0 : 1;
    }

}

int PlayGridSystem::GetOtherUnitDamage(const std::array<UnitState, 4>& newUnitStates, int mainUnit)
{
    int damageU = 0;
    for (int i = 0; i < newUnitStates.size(); i++)
    {
        if (i == mainUnit) continue;
        Dirty_US dirty = Diff_US(m_UnitStates[i], newUnitStates[i]);
        if (HasThis_US(dirty, Dirty_US::hp))
        {
            damageU = m_UnitStates[i].hp - newUnitStates[i].hp;
            break;
        }  
    }
    return damageU;
}


std::wstring PlayGridSystem::GetWeaponFileName(int weaponID)
{   
    std::wstring filaName;
    switch (weaponID)
    {
    case 1:
        filaName = L"../Assets/fbx/weapon/Blaster/Blaster.fbx";
        break;
    case 3:
        filaName = L"../Assets/fbx/weapon/Breacher/Breacher.fbx";
        break;
    case 4:
        filaName = L"../Assets/fbx/weapon/Scythe/Scythe.fbx";
        break;
    case 5:
        filaName = L"../Assets/fbx/weapon/Impactor/Impactor.fbx";
        break;
    case 6:
        filaName = L"../Assets/fbx/weapon/Cleaver/Cleaver.fbx";
        break;
    }
    return filaName;
}

std::wstring PlayGridSystem::GetTileFileName(int num)
{
    const int floorNum = num;
    wchar_t buf[256];
    swprintf_s(buf, L"../Assets/fbx/Tile/floor%d.fbx", floorNum);
    return buf;
}

void PlayGridSystem::SetTileInitState(UnitTile*& pTile, int floornum)
{
    wchar_t idle[256];
    swprintf_s(idle, L"../Assets/fbx/Tile/floor%d.fbx", floornum);
    pTile->AddAnimationClip("idle", idle);

    wchar_t crash[256];
    swprintf_s(crash, L"../Assets/fbx/Tile/Tile_Anim/Floor%d_crash_Anim.fbx", floornum);
    pTile->AddAnimationClip("crash", crash);

    wchar_t wave[256];
    swprintf_s(wave, L"../Assets/fbx/Tile/Tile_Anim/Floor%d_Wave_Anim.fbx", floornum);
    pTile->AddAnimationClip("wave", wave);
}


void PlayGridSystem::ClearTileState()
{
    for (auto& cell : m_tiles)
    {
        cell = TileState{};
    }
}


