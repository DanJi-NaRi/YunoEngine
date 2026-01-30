#include "pch.h"

#include "GridFactory.h"
#include "PieceQueue.h"

#include "ObjectManager.h"
#include "GameManager.h"

#include "GridBox.h"
#include "GridLine.h"
#include "Tile.h"
#include "Piece.h"

#include "PlayGridSystem.h"


PlayGridSystem::PlayGridSystem(ObjectManager* objmng) : GridSystem(objmng) 
{
    m_playGridQ = std::make_unique<PlayGridQ>();
}

PlayGridSystem::~PlayGridSystem()
{
}

void PlayGridSystem::Init(int row, int column, float cellSizeX, float cellSizeZ)
{
    GridSystem::Init(row, column, cellSizeX, cellSizeZ);
    m_tiles = std::vector(m_column * m_row + 1, TileState{});
}

void PlayGridSystem::Update(float dt)
{
    if (!m_playGridQ->Empty())
    {
        PGridCmd cmd = m_playGridQ->Pop();
        
        switch (cmd.cmdType)
        {
        case CommandType::Move:
            {
                const GamePiece& pieceType = cmd.mv_s.whichPiece;
                int cx = cmd.mv_s.cx;
                int cz = cmd.mv_s.cz;
                
                MoveEvent(pieceType, cx, cz);
                break;
            }

        case CommandType::Attack:
            {
                const GamePiece pieceType = cmd.atk_s.whichPiece;
                if (!CheckExisting(pieceType)) return;  // 죽어서 없어진 기물인지 확인

                // 공격 시행 팀 확인. 피격 팀 확인하기.
                Team attackTeam = m_pieces[pieceType].team;

                float damage = cmd.atk_s.damage;
                uint8_t* tileIDs = cmd.atk_s.tileIDs;
                
                for (uint8_t i = 0; i < m_row * m_column; i++)
                {
                    int tileID = tileIDs[i];
                    if (tileID == 0) break;
                    auto& tileTO = m_tiles[tileID].to;
                    TileOccuType targetTeam = tileTO.occuType;

                    bool condition1 = attackTeam == Team::Ally && targetTeam == TileOccuType::Enemy_Occupied;
                    bool condition2 = attackTeam == Team::Enemy && targetTeam == TileOccuType::Ally_Occupied;

                    // 타격 타일 표시
                    auto* pTile = dynamic_cast<Tile*>(m_objectManager->FindObject(m_tilesIDs[tileID]));
                    

                    if (condition1 || condition2)
                    {
                        // 피격 타일 위에 있는 기물
                        PieceInfo& pieceInfo = m_pieces[(GamePiece)tileTO.who];

                        pieceInfo.health -= damage;
                        CheckHealth(pieceInfo);
                        std::cout << "Attack Event Happend. Now Health: " << pieceInfo.health << std::endl;
                    }
                }
                
                break;
            }
        case CommandType::Hit:
            {
            const GamePiece pieceType = cmd.hit.whichPiece;
            int damage = cmd.hit.damage1;
            auto& pieceInfo = m_pieces[pieceType];
            pieceInfo.health -= damage;
            CheckHealth(pieceInfo);
            std::cout << pieceInfo.id <<". health: " << pieceInfo.health << std::endl;
                break;
            }
            
        case CommandType::Dead:
            {
                const GamePiece pieceType = cmd.die_s.whichPiece;
                m_objectManager->DestroyObject(m_pieces[pieceType].id);
                m_pieces.erase(pieceType);
                break;
            }
        }
    }
}

void PlayGridSystem::CreateObject(float x, float y, float z)
{
    CreateGridBox(x, y, z);
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
        auto [wx, wz] = CellToWorld(i % m_column, i / m_column);
        auto pTile = m_objectManager->CreateObject<Tile>(L"Tile", XMFLOAT3(wx, y, wz));
        //auto pTile = m_objectManager->CreateObjectFromFile<Piece>(L"Tile", XMFLOAT3(wx, y, wz), L"../Assets/fbx/Tile.fbx");
        pTile->SetScale({ m_cellSizeX * 0.8f, 1, m_cellSizeZ * 0.8f });
        m_tilesIDs.push_back(pTile->GetID());

        // 빈 박스에 자식 객체로 등록. (for 정리)
        m_gridBox->Attach(pTile);
    }

    // 기물 생성
    const auto& wData = GameManager::Get().GetWeaponData();
    m_wy = y;
    int cx = 0; int cz = 0;
    Team team = Team::Undefined;
    Direction dir;

    for (const auto& w : wData)
    {
        team = (w.pId == GameManager::Get().GetSlotiIdx()) ? Team::Ally : Team::Enemy;

        dir = (w.pId == 1) ? Direction::Right : Direction::Left;
        auto cellPos = GetCellByID(w.currentTile);
        cx = cellPos.x;     cz = cellPos.z;

        auto [px, pz] = CellToWorld(cx, cz);

        // 타일 상태 갱신
        m_tiles[w.currentTile].to = (team == Team::Ally) ?
            TileOccupy{ TileOccuType::Ally_Occupied, (w.slotId == 1) ? TileWho::Ally1 : TileWho::Ally2 } :
            TileOccupy{ TileOccuType::Enemy_Occupied, (w.slotId == 1) ? TileWho::Enemy1 : TileWho::Enemy2 };

        GamePiece gp = (GamePiece)m_tiles[w.currentTile].to.who;

        std::wstring fileName = GetWeaponFileName(w.weaponId);
        auto pPiece = m_objectManager->CreateObject<Piece>(L"Piece", XMFLOAT3(px, m_wy, pz));
        //auto pPiece = m_objectManager->CreateObjectFromFile<Piece>(L"Weapon", XMFLOAT3(px, m_wy, pz), fileName);
        pPiece->SetWho(gp);
        pPiece->SetScale({ 0.8f, 0.8f, 0.8f });
        pPiece->SetDir(dir, false);

        // 기물 정보 등록
        m_pieces.emplace(gp, PieceInfo{ cx, cz, pPiece->GetID(), w.hp, dir, team });

        // 빈 박스에 자식 객체로 등록. (for 정리)
        m_gridBox->Attach(pPiece);
    }
}

void PlayGridSystem::MoveEvent(const GamePiece& pieceType, int cx, int cz)
{
    // 죽어서 없어진 기물인지 확인
    if (!CheckExisting(pieceType)) return;

    // 이동 후, 상태 수정을 위해 참조로 받음
    PieceInfo& pieceInfo = m_pieces[pieceType];

    // 아이디로 오브젝트 포인터 받아오기
    Unit* pUnit = m_objectManager->FindObject(pieceInfo.id);
    Piece* pPiece = dynamic_cast<Piece*>(pUnit);

    // 기물의 기존 좌표
    int oldcx = pieceInfo.cx;
    int oldcz = pieceInfo.cz;

    // 기존 타일과 이동할 타일의 상태
    const TileOccupy oldTo = GetTileTO(oldcx, oldcz);
    const TileOccupy to = GetTileTO(cx, cz);

    // 기물의 회전 방향 체크 및 변경
    auto dir = GetDir(oldcx, oldcz, cx, cz);
    pieceInfo.dir = (dir == Direction::Same) ? pieceInfo.dir : dir;

    // 타 기물과 충돌했을 경우 충돌지점
    auto collDir = GetCollisionDir(oldcx, oldcz, cx, cz);
    auto [colX, colZ] = GetCollisionPos(collDir, pieceInfo.dir, cx, cz);

    // 
    auto [wx, wz] = CellToWorld(cx, cz);

    TileWho who = static_cast<TileWho>(pieceType);
    
    if (oldcx == cx && oldcz == cz)                          // 본인 자리
    {
        std::cout << "[PlayGridSystem]::It's Already there\n";
        return;
    }

    else if (to.occuType == TileOccuType::Collapesed)       // 없어진 자리
    {
        std::cout << "[PlayGridSystem]::Collapesed\n";

        return;
    }
    else if(to.occuType == TileOccuType::Ally_Occupied)     // 아군 존재
    {
        std::cout << "[PlayGridSystem]::Ally_Occupied\n";

        // 충돌지점까지 이동 후 원래 자리로 되돌아감
        pPiece->InsertQ(PlayGridQ::Move_P(dir, colX, m_wy, colZ));
        auto [oldwx, oldwz] = CellToWorld(oldcx, oldcz);
        pPiece->InsertQ(PlayGridQ::Move_P(Direction::Same, oldwx, m_wy, oldwz, 1));

    }
    else if (to.occuType == TileOccuType::Enemy_Occupied)
    {                                                       // 적군 존재
        std::cout << "[PlayGridSystem]::Enemy_Occupied\n";

        // 부딪힌 기물 타입 확인
        GamePiece existWho = (GamePiece)m_tiles[GetID(cx, cz)].to.who;

        // 충돌지점까지 이동 후 원래 자리로 되돌아감
        auto [oldwx, oldwz] = CellToWorld(oldcx, oldcz);
        pPiece->InsertQ(PlayGridQ::Move_P(dir, colX, m_wy, colZ));                  // 충돌 위치까지 이동 후
        pPiece->InsertQ(PlayGridQ::Hit_P(10, existWho, 5));                                      // 피 감소
        pPiece->InsertQ(PlayGridQ::Move_P(Direction::Same, oldwx, m_wy, oldwz, 1)); // 제자리로 돌아감
        
        return;
    }
    else                                                    // 비어있는 자리
    {
        std::cout << "[PlayGridSystem]::Unoccupied\n";
        
        // 기물 이동
        pPiece->InsertQ(PlayGridQ::Move_P(dir, wx, m_wy, wz, 1));

        // 타일 상태 변경
        ChangeTileTO(oldcx, oldcz, TileOccupy{ TileOccuType::Unoccupied, TileWho::None });
        ChangeTileTO(cx, cz,
            (pieceInfo.team == Team::Ally) ?
            TileOccupy{ TileOccuType::Ally_Occupied, who } :
            TileOccupy{ TileOccuType::Enemy_Occupied, who });

        // 기물 정보 변경
        pieceInfo.cx = cx;   pieceInfo.cz = cz;
        return;
    }
    
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

void PlayGridSystem::CheckHealth(PieceInfo& pieceInfo)
{
    if (pieceInfo.health <= 0)
    {
        // 해당 기물 렌더X
        auto pPiece = dynamic_cast<Piece*>(m_objectManager->FindObject(pieceInfo.id));

        pPiece->SetDead();

        // 해당 타일 정보 초기화
        int tileID = GetID(pieceInfo.cx, pieceInfo.cz);
        m_tiles[tileID] = TileState{};

        // 해당 기물 정보 체력 0으로 보정
        pieceInfo.health = 0;
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

Direction PlayGridSystem::GetDir(float oldcx, float oldcz, float cx, float cz)
{
    int dx = cx - oldcx;

    if (dx > 0)  return Direction::Right;
    if (dx < 0)  return Direction::Left;
    return Direction::Same;
}

Direction PlayGridSystem::GetConverseDir(Direction dir)
{
    return (dir == Direction::Right) ? Direction::Left : Direction::Right;
}

Direction PlayGridSystem::GetCollisionDir(float oldcx, float oldcz, float cx, float cz)
{
    int dx = (cx > oldcx) - (cx < oldcx);   // -1, 0, 1
    int dz = (cz > oldcz) - (cz < oldcz);   // -1, 0, 1

    if (dx == 0) return (dz < 0) ? Direction::Up : Direction::Down;
    if (dz == 0) return (dx > 0) ? Direction::Right : Direction::Left;

    // 대각선
    if (dx > 0) return (dz < 0) ? Direction::UpRight : Direction::DownRight;
    else        return (dz < 0) ? Direction::UpLeft : Direction::DownLeft;

}

F2 PlayGridSystem::GetCollisionPos(Direction dir, Direction pieceDir, int cx, int cz)
{
    auto [wx, wz] = CellToWorld(cx, cz);
    float width = (m_cellSizeX / 2.f);
    float hight = (m_cellSizeZ / 2.f);

    float newX, newZ;   newX = newZ = 0;

    int inverse = (pieceDir == Direction::Right) ? 1 : -1;

    switch (dir)
    {
    case Direction::Up:
        newX = wx;
        newZ = wz - hight;
        break;
    case Direction::Down:
        newX = wx;
        newZ = wz + hight;
        break;
    case Direction::Right:
        newX = wx - (width * inverse);
        newZ = wz;
        break;
    case Direction::Left:
        newX = wx - (width * inverse);
        newZ = wz;
        break;
    case Direction::UpLeft:
        newX = wx - (width * inverse);
        newZ = wz - hight;
        break;
    case Direction::UpRight:
        newX = wx - (width * inverse);
        newZ = wz - hight;
        break;
    case Direction::DownLeft:
        newX = wx - (width * inverse);
        newZ = wz + hight;
        break;
    case Direction::DownRight:
        newX = wx - (width * inverse);
        newZ = wz + hight;
        break;
    }
    return { newX, newZ };
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


void PlayGridSystem::ClearTileState()
{
    for (auto& cell : m_tiles)
    {
        cell = TileState{};
    }
}


