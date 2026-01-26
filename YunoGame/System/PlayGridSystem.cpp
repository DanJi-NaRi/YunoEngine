#include "pch.h"

#include "GridFactory.h"
#include "PieceQueue.h"

#include "ObjectManager.h"

#include "GridLine.h"
#include "Tile.h"
#include "Piece.h"

#include "PlayGridSystem.h"


PlayGridSystem::PlayGridSystem(ObjectManager* objmng) : GridSystem(objmng) 
{
    m_pieceQ = std::make_unique<PieceQ>();
}

PlayGridSystem::~PlayGridSystem()
{
}

void PlayGridSystem::Update(float dt)
{
    if (!m_pieceQ->Empty())
    {
        PieceCmd cmd = m_pieceQ->Pop();
        
        switch (cmd.cmdType)
        {
        case CommandType::Move:
        {
            const PieceType& pieceType = cmd.mv_s.whichPiece;
            int cx = cmd.mv_s.cx;
            int cz = cmd.mv_s.cz;
            
            MoveEvent(pieceType, cx, cz);
            break;
        }
            
        }
    }
}

void PlayGridSystem::MoveEvent(const PieceType& pieceType, int cx, int cz)
{
    PieceInfo& pieceInfo = m_pieces[pieceType];                 // 해당 piece의 수정을 위해 참조로 받음
    Unit* pUnit = m_objectManager->FindObject(pieceInfo.id);
    Piece* pPiece = dynamic_cast<Piece*>(pUnit);

    int oldcx = pieceInfo.cx;
    int oldcz = pieceInfo.cz;

    const TileOccupy oldTo = GetTileTO(oldcx, oldcz);
    const TileOccupy to = GetTileTO(cx, cz);
    TileOccuType ot = TileOccuType::Unoccupied;

    auto dir = GetDir(oldcx, oldcz, cx, cz);
    pieceInfo.dir = (dir == Direction::Same) ? pieceInfo.dir : dir;

    auto collDir = GetCollisionDir(oldcx, oldcz, cx, cz);
    auto [colX, colZ] = GetCollisionEnterPos(collDir, pieceInfo.dir, cx, cz);
    auto [exX, exZ] = GetCollisionExistPos(collDir, pieceInfo.dir, cx, cz);

    auto [wx, wz] = CellToWorld(cx, cz);

    TileWho who = static_cast<TileWho>(pieceType);
    
    if (oldcx == cx && oldcz == cz)                          // 본인 자리
    {
        std::cout << "[PlayGridSystem]::It's Already there\n";
        return;
    }
    else if (oldTo.occuType == TileOccuType::Both_Occupied)  // 둘 이상 차지
    {
        std::cout << "[PlayGridSystem]::Both_Occupied\n";

        auto remainWho = (oldTo.mainWho != (TileWho)pieceType) ? oldTo.mainWho : oldTo.subWho;
        auto& remainer = m_pieces[(PieceType)remainWho];
        int id = remainer.id;
        remainer.team;
        Piece* otherP = dynamic_cast<Piece*>(m_objectManager->FindObject(id));

        auto [oldwx, oldwz] = CellToWorld(oldcx, oldcz);
        otherP->InsertQ(PieceQ::Move_P(Direction::Same, oldwx, m_wy, oldwz));

        pPiece->InsertQ(PieceQ::Move_P(dir, wx, m_wy, wz));

        ChangeTileTO(oldcx, oldcz,
            (remainer.team == Team::Ally) ?
            TileOccupy{ TileOccuType::Ally_Occupied, remainWho } :
            TileOccupy{ TileOccuType::Enemy_Occupied, remainWho });
        ChangeTileTO(cx, cz,
            (pieceInfo.team == Team::Ally) ?
            TileOccupy{ TileOccuType::Ally_Occupied, who } :
            TileOccupy{ TileOccuType::Enemy_Occupied, who });
        pieceInfo.cx = cx;   pieceInfo.cz = cz;
        return;
    }
    else if (to.occuType == TileOccuType::Collapesed)       // 없어진 자리
    {
        std::cout << "[PlayGridSystem]::Collapesed\n";

        return;
    }
    else if (to.occuType == TileOccuType::Enemy_Occupied)   // 적군 자리
    {
        std::cout << "[PlayGridSystem]::Enemy_Occupied\n";

        return;
    }
    else if (to.occuType == TileOccuType::Ally_Occupied)    // 아군 자리
    {
        std::cout << "[PlayGridSystem]::Ally_Occupied\n";

        pPiece->InsertQ(PieceQ::Move_P(dir, colX, m_wy, colZ));

        auto mainWho = m_tiles[GetID(cx, cz)].to.mainWho;
        PieceType w = static_cast<PieceType>(mainWho);
        Piece* otherP = dynamic_cast<Piece*>(m_objectManager->FindObject(m_pieces[w].id));

        Direction newDir = GetConverseDir(pieceInfo.dir);
        otherP->InsertQ(PieceQ::Move_P(GetConverseDir(pieceInfo.dir), exX, m_wy, exZ));

        ChangeTileTO(oldcx, oldcz, TileOccupy{ TileOccuType::Unoccupied, TileWho::None });
        ChangeTileTO(cx, cz, TileOccupy{ TileOccuType::Both_Occupied, mainWho, who });
        
        m_pieces[w].dir = newDir;
        pieceInfo.cx = cx;   pieceInfo.cz = cz;
        return;
    }
    else                                                    // 비어있는 자리
    {
        std::cout << "[PlayGridSystem]::Unoccupied\n";
        
        pPiece->InsertQ(PieceQ::Move_P(dir, wx, m_wy, wz));

        ChangeTileTO(oldcx, oldcz, TileOccupy{ TileOccuType::Unoccupied, TileWho::None });
        ChangeTileTO(cx, cz,
            (pieceInfo.team == Team::Ally) ?
            TileOccupy{ TileOccuType::Ally_Occupied, who } :
            TileOccupy{ TileOccuType::Enemy_Occupied, who });
        pieceInfo.cx = cx;   pieceInfo.cz = cz;
        return;
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

F2 PlayGridSystem::GetCollisionEnterPos(Direction dir, Direction pieceDir, int cx, int cz)
{
    auto [wx, wz] = CellToWorld(cx, cz);
    float width = (m_cellSizeX / 2.f) - (m_cellSizeX / 8.f);
    float hight = (m_cellSizeZ / 2.f) - (m_cellSizeZ / 8.f);

    float newX, newZ;   newX = newZ = 0;

    int inverse = (pieceDir == Direction::Right) ? 1 : -1;

    switch (dir)
    {
    case Direction::Up:
        newX = wx + (width * inverse);
        newZ = wz + hight;
        break;
    case Direction::Down:
        newX = wx - (width * inverse);
        newZ = wz + hight;
        break;
    case Direction::Right:
        newX = wx - (width * inverse);
        newZ = wz + hight;
        break;
    case Direction::Left:
        newX = wx - (width * inverse);
        newZ = wz + hight;
        break;
    case Direction::UpLeft:
        newX = wx + (width * inverse);
        newZ = wz + hight;
        break;
    case Direction::UpRight:
        newX = wx + (width * inverse);
        newZ = wz + hight;
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

F2 PlayGridSystem::GetCollisionExistPos(Direction dir, Direction pieceDir, int cx, int cz)
{
    auto [wx, wz] = CellToWorld(cx, cz);
    float width = (m_cellSizeX / 2.f) - (m_cellSizeX / 8.f);
    float hight = (m_cellSizeZ / 2.f) - (m_cellSizeZ / 8.f);

    float newX, newZ;   newX = newZ = 0;

    int inverse = (pieceDir == Direction::Right) ? 1 : -1;

    switch (dir)
    {
    case Direction::Up:
        newX = wx - (width * inverse);
        newZ = wz - hight;
        break;
    case Direction::Down:
        newX = wx + (width * inverse);
        newZ = wz - hight;
        break;
    case Direction::Right:
        newX = wx + (width * inverse);
        newZ = wz - hight;
        break;
    case Direction::Left:
        newX = wx + (width * inverse);
        newZ = wz - hight;
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
        newX = wx + (width * inverse);
        newZ = wz - hight;
        break;
    case Direction::DownRight:
        newX = wx + (width * inverse);
        newZ = wz - hight;
        break;
    }
    return { newX, newZ };
}


void PlayGridSystem::Init(int row, int column, float cellSizeX, float cellSizeZ)
{
    GridSystem::Init(row, column, cellSizeX, cellSizeZ);
    m_tiles = std::vector(m_column * m_row, TileState{});
}


void PlayGridSystem::ClearTileState()
{
    for (auto& cell : m_tiles)
    {
        cell = TileState{};
    }
}


void PlayGridSystem::CreateObject(float x, float y, float z)
{
    CreateGridLine(x, y, z);

    // 지금은 타일 매쉬와 메테리얼을 클래스 내부에서 수동으로 넣어주고 있지만
    // 후에 아트로부터 리소스를 받으면 CreateObject -> CreateObjectFromFile로 바뀌며 자동으로 로드됩니다.
    for (int i = 0; i < m_tiles.size(); i++)
    {
        auto [wx, wz] = CellToWorld(i % m_column, i / m_column);
        auto pTile = m_objectManager->CreateObject<Tile>(L"Tile", XMFLOAT3(wx, y, wz));
        pTile->SetScale({ m_cellSizeX * 0.5f, 1, m_cellSizeZ * 0.5f });
        m_tilesIDs.push_back(pTile->GetID());
    }

    m_wy = y;
    for (auto i = PieceType::Ally1; i < PieceType::MAX; ++i)
    {
        int cx = 0; int cz = 0;
        TileOccupy to{};
        Team team = Team::Undefined;
        Direction dir;
    
        switch (i)
        {
        case PieceType::Ally1:
            cx = 1; cz = 1;
            to = TileOccupy{ TileOccuType::Ally_Occupied, TileWho::Ally1 };
            team = Team::Ally;
            dir = Direction::Right;
            break;
        case PieceType::Ally2:
            cx = 1; cz = 3;
            to = TileOccupy{ TileOccuType::Ally_Occupied, TileWho::Ally2 };
            team = Team::Ally;
            dir = Direction::Right;
            break;
        case PieceType::Enemy1:
            cx = 5; cz = 1;
            to = TileOccupy{ TileOccuType ::Enemy_Occupied, TileWho::Enemy1 };
            team = Team::Enemy;
            dir = Direction::Left;
            break;
        case PieceType::Enemy2:
            cx = 5; cz = 3;
            to = TileOccupy{ TileOccuType::Enemy_Occupied, TileWho::Enemy2 };
            team = Team::Enemy;
            dir = Direction::Left;
            break;
        }
        ChangeTileTO(cx, cz, to);
        
        auto [px, pz] = CellToWorld(cx, cz);
        auto pPiece = m_objectManager->CreateObject<Piece>(L"Piece", XMFLOAT3(px, m_wy, pz));
        //auto pPiece = m_objectManager->CreateObjectFromFile<Piece>(L"Drill", XMFLOAT3(4, 2, 0), L"../Assets/fbx/Drill/Drill.fbx");
        pPiece->SetScale({ 0.5f, 0.5f, 0.5f });
        pPiece->SetDir(dir, false);
        m_pieces.emplace(i, PieceInfo{ cx, cz, pPiece->GetID(), dir, team });
    }
   
}

