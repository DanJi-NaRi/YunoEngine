#include "pch.h"

#include "GridFactory.h"
#include "PlayQueue.h"

#include "ObjectManager.h"
#include "GameManager.h"

#include "Grid.h"
#include "UnitGridBox.h"
#include "UnitGridLine.h"
#include "UnitTile.h"
#include "UnitPiece.h"

#include "PlayGridSystem.h"


PlayGridSystem::PlayGridSystem(ObjectManager* objmng) : UnitGridSystem(objmng)
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
            CreateGrid(5, 7, 3, 3);
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

void PlayGridSystem::Update(float dt)
{
    UpdateAttackSequence(dt);
    CheckPacket(dt);
    CheckMyQ();
}

void PlayGridSystem::CreateObject(float x, float y, float z)
{
    CreateGridBox(x, y, z);

    // 그리드 생성 시, factory 초기화 필요
    // 복수 그리드 생성 시 해당 그리드 정보로 factory 초기화 후, [x y z] 간격 설정하여 CreateGridLine() 호출해주기
    m_gridFactory->Init(m_grids[(int)m_nowG]->m_row, m_grids[(int)m_nowG]->m_column);
    CreateGridLine(x, y, z);

    CreateTileAndPiece(x, y, z);
}

void PlayGridSystem::CreateTileAndPiece(float x, float y, float z)
{
    if (m_gridBox == nullptr) return;

    // 서버와 수 체계를 맞추기 위해.
    m_tilesIDs.push_back({});

    // 지금은 타일 매쉬와 메테리얼을 클래스 내부에서 수동으로 넣어주고 있지만
    // 후에 아트로부터 리소스를 받으면 CreateObject -> CreateObjectFromFile로 바뀌며 자동으로 로드됩니다.
    for (int i = 0; i < m_tiles.size() - 1; i++)
    {
        auto [wx, wz] = m_grids[(int)m_nowG]->CellToWorld(i % m_column, i / m_column);
        //auto pTile = m_manager->CreateObject<UnitTile>(L"Tile", XMFLOAT3(wx, y, wz));
        auto fileName = GetTileFileName(i);
        auto pTile = m_manager->CreateObjectFromFile<UnitTile>(L"Tile", XMFLOAT3(wx, y, wz), fileName);
        pTile->SetScale({ m_cellSizeX * 0.8f, 1, m_cellSizeZ * 0.8f });
        m_tilesIDs.push_back(pTile->GetID());

        // 빈 박스에 자식 객체로 등록. (for 정리)
        m_gridBox->Attach(pTile);
    }

    // 기물 생성
    const auto& wData = GameManager::Get().GetWeaponData();
    m_wy = y;
    int cx = 0; int cy = 0;
    Team team = Team::Undefined;
    Direction dir;
    m_pID = GameManager::Get().GetSlotiIdx();   // 나 몇번째 플레이어니?
    for (const auto& w : wData)
    {
        team = (w.pId == m_pID) ? Team::Ally : Team::Enemy;

        dir = (w.pId == 1) ? Direction::Right : Direction::Left;
        auto cellPos = GetCellByID(w.currentTile);
        cx = cellPos.x;     cy = cellPos.y;

        auto [px, pz] = m_grids[(int)m_nowG]->CellToWorld(cx, cy);

        // 타일 상태 갱신
        m_tiles[w.currentTile].to = (team == Team::Ally) ?
            TileOccupy{ TileOccuType::Ally_Occupied, (w.slotId == 1) ? TileWho::Ally1 : TileWho::Ally2 } :
            TileOccupy{ TileOccuType::Enemy_Occupied, (w.slotId == 1) ? TileWho::Enemy1 : TileWho::Enemy2 };

        GamePiece gp = (GamePiece)m_tiles[w.currentTile].to.who;

        std::wstring fileName = GetWeaponFileName(w.weaponId);
        //auto pPiece = m_manager->CreateObject<UnitPiece>(L"Piece", XMFLOAT3(px, m_wy, pz));
        auto pPiece = m_manager->CreateObjectFromFile<UnitPiece>(L"Weapon", XMFLOAT3(px, m_wy, pz), L"../Assets/fbx/Ax/Ax.fbx");
        //pPiece->AddAnimationClip("move", L"../Animation/move/scythe_move.fbx");
        //pPiece->SetLoop("move", true);
        // weapon 구별을 위한 임시
        switch (w.weaponId)
        {
        case 1:
            pPiece->SetTmpColor({ 1, 0, 0, 1 });
            pPiece->SetEmissiveColor(2, { 1, 0, 0, 1 });       // 빨
            break;
        case 2:
            pPiece->SetTmpColor({ 1, 0.5f, 0, 1 });
            pPiece->SetEmissiveColor(2, { 1, 0.5f, 0, 1 });    // 주
            break;
        case 3:
            pPiece->SetTmpColor({ 1, 1, 0, 1 });
            pPiece->SetEmissiveColor(2, { 1, 1, 0, 1 });       // 노
            break;
        case 4:
            pPiece->SetTmpColor({ 0, 1, 0, 1 });
            pPiece->SetEmissiveColor(2, { 0, 1, 0, 1 });       // 초
            break;
        case 5:
            pPiece->SetTmpColor({ 0, 0, 1, 1 });
            pPiece->SetEmissiveColor(2, { 0, 0, 1, 1 });       // 파
            break;
        case 6:
            pPiece->SetTmpColor({ 0.5f, 0, 0.5f, 1 });
            pPiece->SetEmissiveColor(2, { 0.5f, 0, 0.5f, 1 }); // 보
            break;
        }
        pPiece->SetWho(gp);
        pPiece->SetScale({ 0.8f, 0.8f, 0.8f });
        pPiece->SetDir(dir, false);

        // 기물 정보 등록
        m_pieces.emplace(gp, PieceInfo{pPiece->GetID(), dir, team });

        // 빈 박스에 자식 객체로 등록. (for 정리)
        m_gridBox->Attach(pPiece);

        int usID = GetUnitID(w.pId, w.slotId);
        m_UnitStates[usID] = {
            (uint8_t)w.pId, (uint8_t)w.slotId, (uint8_t)w.hp,
            (uint8_t)w.stamina, (uint8_t)w.currentTile, 0
        };
    }
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
            const GamePiece pieceType = cmd.hit.whichPiece;

            auto& pieceInfo = m_pieces[pieceType];
            int uid = GetUnitID(pieceType);

            CheckHealth(m_UnitStates[uid], pieceInfo);
            std::cout << static_cast<int>(pieceType) << ". health: " << m_UnitStates[uid].hp << std::endl;
            break;
        }

        case CommandType::Dead:
        {
            const GamePiece pieceType = cmd.die_s.whichPiece;

            //auto it = m_pieces.find(pieceType);
            //if (it == m_pieces.end())    break;

            m_manager->DestroyObject(m_pieces[pieceType].id);
            m_pieces.erase(pieceType);
            break;
        }
        }
    }
}

void PlayGridSystem::CheckPacket(float dt)
{
    // 게임 매니저에서 배틀 패킷 하나씩 받아옴
    auto& mng = GameManager::Get();
    if (!mng.IsEmptyBattlePacket() && !isProcessing)
    {
        const auto pckt = mng.PopBattlePacket();
        isProcessing = true;
        m_pktTime = pckt.actionTime / 1000.f; //임의값 넣어둠

        const auto runTimeCardID = pckt.runTimeCardID;
        const auto dir = pckt.dir;
        const std::vector<std::array<UnitState, 4>>& order = pckt.order;

        GamePiece whichPiece = GetGamePiece(pckt.pId, pckt.slotId);
        int mainUnit = GetUnitID(pckt.pId, pckt.slotId);

        // runtimeCardID로 CardManager에서 카드 정보 얻어옴
        const auto& cardData = mng.GetCardData(runTimeCardID);

        const int effectID = cardData.m_effectId;
        const int soundID = cardData.m_soundId;

        const RangeData* rangeData = mng.GetRangeData(runTimeCardID);
        const CardType cardType = cardData.m_type;
        if (cardType == CardType::Attack)    m_pktTime = 7.f;
        
        ApplyActionOrder(order, mainUnit, cardType, rangeData, (Direction)dir);
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
        }
    }
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

        int id = m_pieces[as.attacker].id;
        auto pPiece = static_cast<UnitPiece*>(m_manager->FindObject(id));
        // 애니메이션 대신 반짝이는 걸로 잠시 대체
        pPiece->SetFlashColor(as.m_attackColor, as.m_flashCount, as.m_flashInterval);
        as.phaseStarted = false;
        
        std::cout << "[Attack Sequence]\nAttacker hp: " << static_cast<int>(m_UnitStates[GetUnitID(as.attacker)].hp) << std::endl;
        break;
    }
    case AttackPhase::TileHit:
    {
        // tile hit effect 시간 끝나고 다음 단계로 이동
        if (as.elapsed >= as.m_tileEffectDuration)
        {
            ++as.attackPhase;
            as.phaseStarted = true;
            as.elapsed = 0;
            break;
        }

        if (!as.phaseStarted) break;

        const auto& tiles = as.tileIDs;
        for (int i = 0; i < tiles.size(); i++)
        {
            int id = tiles[i];
            auto pTile = static_cast<UnitTile*>(m_manager->FindObject(m_tilesIDs[id]));
            pTile->SetFlashColor(as.m_tileEffectColor, as.m_flashCount, as.m_flashInterval);
        }
        as.phaseStarted = false;
        break;
    }    
    case AttackPhase::UnitHit:
    {
        // unit hit 시간 끝나고 다음 단계로 이동
        if (as.elapsed >= as.m_hitDuration)
        {
            ++as.attackPhase;
            as.phaseStarted = true;
            as.elapsed = 0;
            break;
        }

        if (!as.phaseStarted) break;

        const auto& pieces = as.hitPieces;
        for (int i = 0; i < pieces.size(); i++)
        {
            if (!CheckExisting(pieces[i]))    continue;
            int id = m_pieces[pieces[i]].id;
            auto pPiece = static_cast<UnitPiece*>(m_manager->FindObject(id));
            // 애니메이션 대신 반짝이는 걸로 잠시 대체
            pPiece->SetFlashColor(as.m_hitColor, as.m_flashCount, as.m_flashInterval);
        }
        as.phaseStarted = false;
        break;
    }
    case AttackPhase::Over:
    {
        // 죽었는지 체크
        const auto& pieces = as.hitPieces;
        for (int i = 0; i < pieces.size(); i++)
        {
            std::cout << "[Attack Sequence]\nHitter hp: " << static_cast<int>(m_UnitStates[GetUnitID(pieces[i])].hp) << std::endl;
            if (!CheckExisting(pieces[i]))    continue;
            auto& pieceinfo = m_pieces[pieces[i]];
            int id = GetUnitID(pieces[i]);
            CheckHealth(m_UnitStates[id], pieceinfo);
        }
        
        as = {};
        m_attackActive = false;
        break;
    }
    }

    as.elapsed += dt;
}

void PlayGridSystem::ApplyActionOrder(const std::vector<std::array<UnitState, 4>>& order, int mainUnit, CardType cardType, const RangeData* rangeData, Direction dir)
{
    // 행동 순서
    for (int i = 0; i < order.size(); i++)
    {
        const std::array<UnitState, 4>& unitStates_Now = order[i];

        Dirty_US dirty = Diff_US(m_UnitStates[mainUnit], unitStates_Now[mainUnit]);

        const auto& ranges = rangeData->offsets;
        switch (cardType)
        {
        case CardType::Buff:
            break;
        case CardType::Move: 
            ApplyMoveChanges(dirty, unitStates_Now, mainUnit, dir);
            break;
        case CardType::Attack:
            ApplyAttackChanges(dirty, unitStates_Now, mainUnit, ranges, dir);
            break;
        case CardType::Utility:
            break;
        }

        m_UnitStates = unitStates_Now;
    }
}

void PlayGridSystem::ApplyMoveChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, Direction dir)
{
    const UnitState prevUS = m_UnitStates[mainUnit];
    const UnitState newUS = newUnitStates[mainUnit];
    GamePiece whichPiece = GetGamePiece(newUS.pId, newUS.slotId);
    if (!CheckExisting(whichPiece))
    {
        // 부활 규칙이 없으므로 return 합니다.
        return;
    }

    auto newcell = GetCellByID(newUS.targetTileID);
    auto oldcell = GetCellByID(prevUS.targetTileID);

    // 이동하다가 충돌했을 때
    bool isCollided = newUS.isEvent;
    if (isCollided)
    {
        if (HasThis_US(dirty, Dirty_US::hp))
        {
            int damageMe = prevUS.hp - newUS.hp;
            int damageU = GetOtherUnitDamage(newUnitStates, mainUnit);
            //m_playQ->Insert(m_playQ->Move_S(whichPiece, oldcx, oldcz, cx, cz, true, true, damageMe, damageU));
            MoveEvent(whichPiece, oldcell, newcell, dir, true, true, damageMe, damageU);
        }
        else
        {
            //m_playQ->Insert(m_playQ->Move_S());
            MoveEvent(whichPiece, oldcell, newcell, dir, true);
        }
    }
    // 이동만 할 때
    else if (HasThis_US(dirty, Dirty_US::targetTileID))
    {
        //m_playQ->Insert(m_playQ->Move_S(whichPiece, oldcx, oldcz, cx, cz));
        MoveEvent(whichPiece, oldcell, newcell, dir);
    }

}

void PlayGridSystem::ApplyAttackChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, const std::vector<RangeOffset>& ranges, Direction dir)
{
    const UnitState prevUS = m_UnitStates[mainUnit];
    const UnitState newUS = newUnitStates[mainUnit];
    GamePiece whichPiece = GetGamePiece(newUS.pId, newUS.slotId);
    if (!CheckExisting(whichPiece))
    {
        // 부활 규칙이 없으므로 return 합니다.
        return;
    }
    Int2 cellPos = { GetCellByID(prevUS.targetTileID) };
    const auto targetTileIDs = GetRangeTileIDs(cellPos, ranges, dir);

    // 공격 시퀀스 채워넣기
    auto& as = m_attackSequence;
    as.attacker = whichPiece;
    as.tileIDs = targetTileIDs;
    as.attackPhase = AttackPhase::Alaram;
    as.phaseStarted = m_attackActive = true;

    // 공격 시행 팀 확인. 피격 팀 확인하기.
    Team attackTeam = m_pieces[whichPiece].team;
    Float4 allyColor = Float4{ 0, 0, 0.8f, 1 };
    Float4 enemyColor = Float4{ 0.8f, 0, 0, 1 };
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

    as.m_flashCount = 3;
    as.m_flashInterval = 0.3f;
    as.m_alarmDuration = as.m_attackDuration = as.m_tileEffectDuration = as.m_hitDuration 
        = as.m_flashCount * as.m_flashInterval;

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
            relativeRanges.push_back({ -range.dy, range.dx });
            break;
        case Direction::Down:
            relativeRanges.push_back({ -range.dy, -range.dx });
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
        absoluteRanges.push_back({ cx, cz });
    }

    for (int i = 0; i < absoluteRanges.size(); i++)
    {
        tileIDs.push_back(GetID(absoluteRanges[i].x, absoluteRanges[i].y));
    }

    return tileIDs;
}

void PlayGridSystem::MoveEvent(const GamePiece& pieceType, Int2 oldcell, Int2 newcell, Direction moveDir,
    bool isCollided, bool isEnemy, int damageMe, int damageU)
{
    // 죽어서 없어진 기물인지 확인
    if (!CheckExisting(pieceType)) return;

    // 이동 후, 상태 수정을 위해 참조로 받음
    PieceInfo& pieceInfo = m_pieces[pieceType];

    // 아이디로 오브젝트 포인터 받아오기
    UnitPiece* pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));

    // 기존 타일과 이동할 타일의 상태
    const TileOccupy oldTo = GetTileTO(oldcell.x, oldcell.y);
    const TileOccupy to = GetTileTO(newcell.x, newcell.y);

    // 기물의 상태 회전 방향 체크 및 변경
    auto dir = Get2Dir(oldcell.x, oldcell.y, newcell.x, newcell.y);
    auto fdir = pieceInfo.dir = (dir == Direction::Same) ? pieceInfo.dir : dir;

    // 타 기물과 충돌했을 경우 충돌지점
    //auto moveDir = Get8Dir(oldcell.x, oldcell.y, newcell.x, newcell.y);             // 진입 방향
    auto [colW, colC] = GetCollisionPos(moveDir, newcell.x, newcell.y);

    // 
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

            // 부딪힌 기물 타입 확인
            GamePiece existWho = GamePiece::None;
            bool isIn = m_grids[(int)m_nowG]->InBounds(colC.x, colC.y);
            if (isIn)
            {
                int tileID = GetID(colC.x, colC.y);
                existWho = (GamePiece)m_tiles[tileID].to.who;
            }

            // 충돌지점까지 이동 후 원래 자리로 되돌아감
            pPiece->InsertQ(PlayGridQ::Move_P(fdir, colW.x, m_wy, colW.y));              // 충돌 위치까지 이동 후
            pPiece->InsertQ(PlayGridQ::Hit_P(damageMe, existWho, damageU));              // 피 감소
            pPiece->InsertQ(PlayGridQ::Move_P(Direction::Same, wx, m_wy, wz, 1));        // 제자리로 돌아감
        }
        else                                                // 아군과 충돌
        {
            std::cout << "[PlayGridSystem]::Ally_Collison\n";

            // 충돌지점까지 이동 후 원래 자리로 되돌아감
            pPiece->InsertQ(PlayGridQ::Move_P(fdir, colW.x, m_wy, colW.y));
            pPiece->InsertQ(PlayGridQ::Move_P(fdir, wx, m_wy, wz, 1));
        }
    }
    else                                                                                // 비어있는 자리
    {
        std::cout << "[PlayGridSystem]::Unoccupied\n";
        
        // 기물 이동
        pPiece->InsertQ(PlayGridQ::Move_P(fdir, wx, m_wy, wz, 1));
    }
    
    // 타일 상태 변경
    ChangeTileTO(oldcell.x, oldcell.y, TileOccupy{ TileOccuType::Unoccupied, TileWho::None });
    ChangeTileTO(newcell.x, newcell.y,
        (pieceInfo.team == Team::Ally) ?
        TileOccupy{ TileOccuType::Ally_Occupied, who } :
        TileOccupy{ TileOccuType::Enemy_Occupied, who });
}

bool PlayGridSystem::CheckExisting(const GamePiece pieceType)
{
    auto pIt = m_pieces.find(pieceType);
    if (pIt == m_pieces.end())
    {
        std::cout << "[PlayGridSystem]::It's Already Dead!!\n";
        return false;
    }
    return true;
}

//void PlayGridSystem::CheckHealth(PieceInfo& pieceInfo)
//{
//    if (pieceInfo.health <= 0)
//    {
//        // 해당 기물 렌더X
//        auto pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));
//
//        pPiece->SetDead();
//
//        // 해당 타일 정보 초기화
//        int tileID = GetID(pieceInfo.cx, pieceInfo.cz);
//        m_tiles[tileID] = TileState{};
//
//        // 해당 기물 정보 체력 0으로 보정
//        pieceInfo.health = 0;
//    }
//}

void PlayGridSystem::CheckHealth(const UnitState& us, PieceInfo& pieceInfo)
{
    if (us.hp <= 0)
    {
        // 해당 기물 렌더X
        auto pPiece = dynamic_cast<UnitPiece*>(m_manager->FindObject(pieceInfo.id));

        pPiece->SetDead();

        // 해당 타일 정보 초기화
        int tileID = m_tilesIDs[us.targetTileID];
        m_tiles[tileID] = TileState{};
    }
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
        filaName = L"../Assets/fbx/Ax/Ax.fbx";
        break;
    case 2:
        filaName = L"../Assets/fbx/Drill/Drill.fbx";
        break;
    case 3:
        filaName = L"../Assets/fbx/LaserGun/LaserGun.fbx";
        break;
    case 4:
        filaName = L"../Assets/fbx/Ax/Ax.fbx";
        break;
    case 5:
        filaName = L"../Assets/fbx/Drill/Drill.fbx";
        break;
    case 6:
        filaName = L"../Assets/fbx/LaserGun/LaserGun.fbx";
        break;
    }
    return filaName;
}

std::wstring PlayGridSystem::GetTileFileName(int tile)
{
    static const int floorOfTile[35] = {
        5,10,15,20,25,30,1,
        4,9,14,19,24,29,2,
        3,8,13,18,23,28,3,
        2,7,12,17,22,27,4,
        1,6,11,16,21,26,5
    };

    const int floorNum = floorOfTile[tile];
    wchar_t buf[256];
    swprintf_s(buf, L"../Assets/fbx/Tile/floor%d.fbx", floorNum);
    return buf;
}


void PlayGridSystem::ClearTileState()
{
    for (auto& cell : m_tiles)
    {
        cell = TileState{};
    }
}


