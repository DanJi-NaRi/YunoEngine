#include "pch.h"

#include "GridFactory.h"
#include "MinimapQueue.h"

#include "UIManager.h"
#include "GameManager.h"
#include "IInput.h"

#include "Grid.h"
#include "WidgetGridBox.h"
#include "WidgetGridLine.h"
#include "WidgetTile.h"
#include "WidgetPiece.h"

#include "MinimapGridSystem.h"

MinimapGridSystem::MinimapGridSystem(UIManager* manager) : WidgetGridSystem(manager)
{
    m_minimapQ = std::make_unique<MinimapQ>();

    Init();
}

MinimapGridSystem::MinimapGridSystem(UIManager* uimng, IInput* input) : WidgetGridSystem(uimng), m_input(input)
{
    assert(m_input != nullptr);

    m_minimapQ = std::make_unique<MinimapQ>();

    Init();
}

MinimapGridSystem::~MinimapGridSystem()
{

}

void MinimapGridSystem::Init()
{
    for (int i = 0; i < static_cast<int>(NG_M::Max); i++)
    {
        switch (static_cast<NG_M>(i))
        {
        case NG_M::One:
        {
            CreateGrid(5, 7, 1.2, 1.2);
            break;
        }
        }
    }

    // m_tiles 초기화
    // 현재 단일 그리드라 m_tiles도 단일 벡터로 존재
    // 복수 그리드가 되는 그리드 시스템에서는 m_tiles는 unordered_map 혹은 벡터의 벡터로 존재해야함
    m_nowG = NG_M::One;
    SetNG_P(m_grids[(int)m_nowG].get());
    m_tiles = std::vector(m_column * m_row + 1, TileState{});
}

void MinimapGridSystem::CreateTileAndPiece(float x, float y, float z)
{
    //if (m_gridBox == nullptr) return;

    // 서버와 수 체계를 맞추기 위해.
    m_tilesIDs.push_back({});

    m_uiScale = 50;     // widget에 있는 scale offset 값

    // 지금은 타일 매쉬와 메테리얼을 클래스 내부에서 수동으로 넣어주고 있지만
    // 후에 아트로부터 리소스를 받으면 CreateObject -> CreateObjectFromFile로 바뀌며 자동으로 로드됩니다.
    for (int i = 0; i < m_tiles.size() - 1; i++)
    {
        auto [wx, wy] = m_grids[(int)m_nowG]->CellToWorld(i % m_column, i / m_column);
        float tx = wx * m_uiScale + x;
        float ty = wy * m_uiScale + y;
        //auto pTile = m_objectManager->CreateObject<Tile>(L"Tile", XMFLOAT3(wx, y, wz));
        auto pTile = m_manager->CreateWidget<WidgetTile>(L"Tile", XMFLOAT3(tx, ty, z));
        pTile->SetScale({ m_cellSizeX * 0.9f, m_cellSizeZ * 0.9f, 1 });
        m_tilesIDs.push_back(pTile->GetID());
        
        // 빈 박스에 자식 객체로 등록. (for 정리)
        //m_gridBox->Attach(pTile);
    }

    
    auto wData = GameManager::Get().GetWeaponData();
    wData.clear();

    // test
    // 서버에서 초기화 정보 넘겨줘야함ㅠ
    wData.push_back({ 1, 1, 1, 100, 100, 9  });
    wData.push_back({ 1, 2, 2, 100, 100, 23 });
    wData.push_back({ 2, 1, 1, 100, 100, 13 });
    wData.push_back({ 2, 2, 2, 100, 100, 27 });

    ////////

    m_wz = z;
    int cx = 0; int cy = 0;
    Team team = Team::Undefined;
    Direction dir;

    // 기물 생성
    for (const auto& w : wData)
    {
        //team = (w.pId == GameManager::Get().GetSlotiIdx()) ? Team::Ally : Team::Enemy;
        team = Team::Ally;//일단!! 테스트를 위해 임의로 이렇게 설정

        dir = (w.pId == 1) ? Direction::Right : Direction::Left;
        auto cellPos = GetCellByID(w.currentTile);
        cx = cellPos.x;     cy = cellPos.y;

        auto [px, py] = m_grids[(int)m_nowG]->CellToWorld(cx, cy);

        float tx = px * m_uiScale + x;
        float ty = py * m_uiScale + y;

        // 타일 상태 갱신
        m_tiles[w.currentTile].to = (team == Team::Ally) ?
            TileOccupy{ TileOccuType::Ally_Occupied, (w.slotId == 1) ? TileWho::Ally1 : TileWho::Ally2 } :
            TileOccupy{ TileOccuType::Enemy_Occupied, (w.slotId == 1) ? TileWho::Enemy1 : TileWho::Enemy2 };

        GamePiece gp = (GamePiece)m_tiles[w.currentTile].to.who;

        //std::wstring fileName = GetWeaponFileName(w.weaponId);
        auto pPiece = m_manager->CreateObject<WidgetPiece>(L"Piece", XMFLOAT3(tx, ty, m_wz));
        //auto pPiece = m_objectManager->CreateObjectFromFile<Piece>(L"Weapon", XMFLOAT3(px, m_wy, pz), fileName);
        pPiece->SetWho(gp);
        pPiece->SetScale({ m_uiScale, m_uiScale, 1 });
        pPiece->SetDir(dir, false);

        // 기물 정보 등록
        m_pieces.emplace(gp, PieceInfo{ pPiece->GetID(), dir, team });

        // 빈 박스에 자식 객체로 등록. (for 정리)
        //m_gridBox->Attach(pPiece);
    }

}

void MinimapGridSystem::CheckMyQ()
{
    //if (!m_minimapQ->Empty())
    //{
    //    PGridCmd cmd = m_minimapQ->Pop();

    //    switch (cmd.cmdType)
    //    {
    //    case CommandType::Move:
    //    {
    //        const GamePiece& pieceType = cmd.mv_s.whichPiece;
    //        int cx = cmd.mv_s.cx;
    //        int cy = cmd.mv_s.cz;

    //        MoveEvent(pieceType, cx, cy);
    //        break;
    //    }

    //    case CommandType::Attack:
    //    {
    //        const GamePiece pieceType = cmd.atk_s.whichPiece;
    //        if (!CheckExisting(pieceType)) return;  // 죽어서 없어진 기물인지 확인

    //        // 공격 시행 팀 확인. 피격 팀 확인하기.
    //        Team attackTeam = m_pieces[pieceType].team;

    //        float damage = cmd.atk_s.damage;
    //        uint8_t* tileIDs = cmd.atk_s.tileIDs;

    //        for (uint8_t i = 0; i < m_row * m_column; i++)
    //        {
    //            int tileID = tileIDs[i];
    //            if (tileID == 0) break;
    //            auto& tileTO = m_tiles[tileID].to;
    //            TileOccuType targetTeam = tileTO.occuType;

    //            bool condition1 = attackTeam == Team::Ally && targetTeam == TileOccuType::Enemy_Occupied;
    //            bool condition2 = attackTeam == Team::Enemy && targetTeam == TileOccuType::Ally_Occupied;

    //            // 타격 타일 표시
    //            auto* pTile = dynamic_cast<WidgetTile*>(m_manager->FindWidget(m_tilesIDs[tileID]));


    //            if (condition1 || condition2)
    //            {
    //                // 피격 타일 위에 있는 기물
    //                PieceInfo& pieceInfo = m_pieces[(GamePiece)tileTO.who];

    //                std::cout << "Attack Event Happend." << std::endl;
    //            }
    //        }

    //        break;
    //    }
    //    case CommandType::Hit:
    //    {
    //        const GamePiece pieceType = cmd.hit.whichPiece;
    //        auto& pieceInfo = m_pieces[pieceType];

    //        break;
    //    }
    //    }
    //}
}

void MinimapGridSystem::CheckInput()
{
    if (m_input->IsMouseButtonReleased(0))
    {
        POINT mouseXY = { (LONG)m_input->GetMouseX(), (LONG)m_input->GetMouseY() };
        auto [wx, wy] = GetwPos(mouseXY.x, mouseXY.y);
        auto [cx, cy] = m_grids[(int)m_nowG]->WorldToCell(wx, wy);
        bool isIn = m_grids[(int)m_nowG]->InBounds(cx, cy);
        if (isIn)
        {
            std::cout << "it's inside of the grid.\n";
        }
        else
            std::cout << "it's outside of the grid.\n";
    }
}

void MinimapGridSystem::CreateObject(float x, float y, float z)
{
    //CreateGridBox(x, y, z);

    // 그리드 생성 시, factory 초기화 필요
    // 복수 그리드 생성 시 해당 그리드 정보로 factory 초기화 후, [x y z] 간격 설정하여 CreateGridLine() 호출해주기
    m_gridFactory->Init(m_grids[(int)m_nowG]->m_row, m_grids[(int)m_nowG]->m_column);
    CreateGridLine(x, y, z);

    CreateTileAndPiece(x, y, z);

    m_vPos = { x, y, z };
}

void MinimapGridSystem::Update(float dt)
{
    CheckMyQ();
    CheckInput();
}

void MinimapGridSystem::MoveEvent(const GamePiece& pieceType, int cx, int cy)
{
    //// 죽어서 없어진 기물인지 확인
    //if (!CheckExisting(pieceType)) return;

    //// 이동 후, 상태 수정을 위해 참조로 받음
    //PieceInfo& pieceInfo = m_pieces[pieceType];

    //// 아이디로 오브젝트 포인터 받아오기
    //Widget* pUnit = m_manager->FindWidget(pieceInfo.id);
    //auto* pPiece = dynamic_cast<WidgetPiece*>(pUnit);

    //// 기물의 기존 좌표
    //int oldcx = pieceInfo.cx;
    //int oldcy = pieceInfo.cz;

    //// 기존 타일과 이동할 타일의 상태
    //const TileOccupy oldTo = GetTileTO(oldcx, oldcy);
    //const TileOccupy to = GetTileTO(cx, cy);

    //// 기물의 회전 방향 체크 및 변경
    //auto dir = GetDir(oldcx, oldcy, cx, cy);
    //pieceInfo.dir = (dir == Direction::Same) ? pieceInfo.dir : dir;

    //// 타 기물과 충돌했을 경우 충돌지점
    //auto collDir = GetCollisionDir(oldcx, oldcy, cx, cy);
    //auto [colX, colY] = GetCollisionPos(collDir, pieceInfo.dir, cx, cy);

    //// 
    //auto [wx, wy] = GetUIwPos(cx, cy);

    //TileWho who = static_cast<TileWho>(pieceType);

    //if (oldcx == cx && oldcy == cy)                          // 본인 자리
    //{
    //    std::cout << "[PlayGridSystem]::It's Already there\n";
    //    return;
    //}

    //else if (to.occuType == TileOccuType::Collapesed)       // 없어진 자리
    //{
    //    std::cout << "[PlayGridSystem]::Collapesed\n";

    //    return;
    //}
    //else if (to.occuType == TileOccuType::Ally_Occupied)     // 아군 존재
    //{
    //    std::cout << "[PlayGridSystem]::Ally_Occupied\n";

    //    // 충돌지점까지 이동 후 원래 자리로 되돌아감
    //    pPiece->InsertQ(MinimapQ::Move_P(dir, colX, colY, m_wz, m_uiScale));
    //    auto [oldwx, oldwy] = GetUIwPos(oldcx, oldcy);
    //    pPiece->InsertQ(MinimapQ::Move_P(Direction::Same, oldwx, oldwy, m_wz, m_uiScale));

    //}
    //else if (to.occuType == TileOccuType::Enemy_Occupied)
    //{                                                       // 적군 존재
    //    std::cout << "[PlayGridSystem]::Enemy_Occupied\n";

    //    // 부딪힌 기물 타입 확인
    //    GamePiece existWho = (GamePiece)m_tiles[GetID(cx, cy)].to.who;

    //    // 충돌지점까지 이동 후 원래 자리로 되돌아감
    //    auto [oldwx, oldwy] = GetUIwPos(oldcx, oldcy);
    //    pPiece->InsertQ(MinimapQ::Move_P(dir, colX, colY, m_wz, m_uiScale));               // 충돌 위치까지 이동 후
    //    pPiece->InsertQ(MinimapQ::Hit_P(10, existWho, 5));                                 // 피 감소
    //    pPiece->InsertQ(MinimapQ::Move_P(Direction::Same, oldwx, oldwy, m_wz, m_uiScale)); // 제자리로 돌아감

    //    return;
    //}
    //else                                                    // 비어있는 자리
    //{
    //    std::cout << "[PlayGridSystem]::Unoccupied\n";

    //    // 기물 이동
    //    pPiece->InsertQ(MinimapQ::Move_P(dir, wx, wy, m_wz, m_uiScale));

    //    // 타일 상태 변경
    //    ChangeTileTO(oldcx, oldcy, TileOccupy{ TileOccuType::Unoccupied, TileWho::None });
    //    ChangeTileTO(cx, cy,
    //        (pieceInfo.team == Team::Ally) ?
    //        TileOccupy{ TileOccuType::Ally_Occupied, who } :
    //        TileOccupy{ TileOccuType::Enemy_Occupied, who });

    //    // 기물 정보 변경
    //    pieceInfo.cx = cx;   pieceInfo.cz = cy;
    //    return;
    //}
}

bool MinimapGridSystem::CheckExisting(const GamePiece pieceType)
{
    auto pIt = m_pieces.find(pieceType);
    if (pIt == m_pieces.end())
    {
        std::cout << "[MinimapGridSystem]::It's Already Dead!!\n";
        return false;
    }
    return true;
}

void MinimapGridSystem::ChangeTileTO(int cx, int cy, const TileOccupy to)
{
    m_tiles[GetID(cx, cy)].to = to;
}

const TileOccupy MinimapGridSystem::GetTileTO(int cx, int cy)
{
    return m_tiles[GetID(cx, cy)].to;
}

Float2 MinimapGridSystem::GetUIwPos(int cx, int cy)
{
    auto [x, y] = m_grids[(int)m_nowG]->CellToWorld(cx, cy);
    float uwx = x * m_uiScale + m_vPos.x;
    float uwy = y * m_uiScale + m_vPos.y;
    return {uwx, uwy};
}

Float2 MinimapGridSystem::GetwPos(float uwx, float uwy)
{
    float wx = (uwx - m_vPos.x) / m_uiScale;
    float wy = (uwy - m_vPos.y) / m_uiScale;
    return { wx, wy };
}

Direction MinimapGridSystem::GetDir(float oldcx, float oldcy, float cx, float cy)
{
    int dx = cx - oldcx;

    if (dx > 0)  return Direction::Right;
    if (dx < 0)  return Direction::Left;
    return Direction::Same;
}

Direction MinimapGridSystem::GetCollisionDir(float oldcx, float oldcy, float cx, float cy)
{
    int dx = (cx > oldcx) - (cx < oldcx);   // -1, 0, 1
    int dy = (cy > oldcy) - (cy < oldcy);   // -1, 0, 1

    if (dx == 0) return (dy < 0) ? Direction::Up : Direction::Down;
    if (dy == 0) return (dx > 0) ? Direction::Right : Direction::Left;

    // 대각선
    if (dx > 0) return (dy < 0) ? Direction::UpRight : Direction::DownRight;
    else        return (dy < 0) ? Direction::UpLeft : Direction::DownLeft;
}

Float2 MinimapGridSystem::GetCollisionPos(Direction dir, Direction pieceDir, int cx, int cy)
{
    auto [wx, wy] = m_grids[(int)m_nowG]->CellToWorld(cx, cy);
    float width = (m_cellSizeX / 2.f);
    float hight = (m_cellSizeZ / 2.f);

    float newX, newY;   newX = newY = 0;

    int inverse = (pieceDir == Direction::Right) ? 1 : -1;

    switch (dir)
    {
    case Direction::Up:
        newX = wx;
        newY = wy + hight;
        break;
    case Direction::Down:
        newX = wx;
        newY = wy - hight;
        break;
    case Direction::Right:
        newX = wx - (width * inverse);
        newY = wy;
        break;
    case Direction::Left:
        newX = wx - (width * inverse);
        newY = wy;
        break;
    case Direction::UpLeft:
        newX = wx - (width * inverse);
        newY = wy + hight;
        break;
    case Direction::UpRight:
        newX = wx - (width * inverse);
        newY = wy + hight;
        break;
    case Direction::DownLeft:
        newX = wx - (width * inverse);
        newY = wy - hight;
        break;
    case Direction::DownRight:
        newX = wx - (width * inverse);
        newY = wy - hight;
        break;
    }
    return { newX * m_uiScale + m_vPos.x, newY * m_uiScale + m_vPos.y };
}
