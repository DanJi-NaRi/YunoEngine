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
    PieceInfo& pieceInfo = m_pieces[pieceType];
    Unit* pUnit = m_objectManager->FindObject(pieceInfo.id);
    Piece* pPiece = dynamic_cast<Piece*>(pUnit);

    const TileOccupancy to = GetTileTO(cx, cz);

    int oldcx = pieceInfo.cx;
    int oldcz = pieceInfo.cz;

    auto dir = GetDir(oldcx, oldcz, cx, cz);
    
    if (oldcx == cx && oldcz == cz)                 // 본인 자리
    {
        std::cout << "[PlayGridSystem]::It's Already there\n";
    }
    else if (to == TileOccupancy::Collapesed)       // 없어진 자리
    {
        std::cout << "[PlayGridSystem]::Collapesed\n";
    }

    else if (to == TileOccupancy::Enemy_Occupied)   // 적군 자리
    {
        std::cout << "[PlayGridSystem]::Enemy_Occupied\n";
    }
    else if (to == TileOccupancy::Ally_Occupied)    // 아군 자리
    {
        std::cout << "[PlayGridSystem]::Ally_Occupied\n";
        
    }
    else                                            // 비어있는 자리
    {
        std::cout << "[PlayGridSystem]::Unoccupied\n";
        auto [wx, wz] = CellToWorld(cx, cz);
        
        pPiece->InsertQ(PieceQ::Move_P(dir, wx, m_wy, wz));

        ChangeTileTO(oldcx, oldcz, TileOccupancy::Unoccupied);
        ChangeTileTO(cx, cz, 
            (pieceInfo.team == Team::Ally) ? TileOccupancy::Ally_Occupied : TileOccupancy::Enemy_Occupied);
        pieceInfo.cx = cx; pieceInfo.cz = cz;
    }
}

void PlayGridSystem::ChangeTileTO(int cx, int cz, const TileOccupancy to)
{
    m_tiles[GetID(cx, cz)].to = to;
}

const TileOccupancy PlayGridSystem::GetTileTO(int cx, int cz)
{
    return m_tiles[GetID(cx, cz)].to;
}

Direction PlayGridSystem::GetDir(float oldcx, float oldcz, float cx, float cz)
{
    if (oldcx == cx)
    {
        return (oldcz > cz) ? Direction::Up : Direction::Down;
    }
    else if (oldcz == cz)
    {
        return (oldcx < cx) ? Direction::Right : Direction::Left;
    }
    else
    {
        bool isRight = (oldcx < cx) ? true : false;
        bool isUp = (oldcz > cz) ? true : false;

        return (isRight) ?
            (isUp) ? Direction::UpRight : Direction::DownRight :
            (isUp) ? Direction::UpLeft : Direction::DownLeft;
    }
    
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
        TileOccupancy to = TileOccupancy::Unoccupied;
        Team team = Team::Undefined;
        Direction dir;
    
        switch (i)
        {
        case PieceType::Ally1:
            cx = 1; cz = 1;
            to = TileOccupancy::Ally_Occupied;
            team = Team::Ally;
            dir = Direction::Right;
            break;
        case PieceType::Ally2:
            cx = 1; cz = 3;
            to = TileOccupancy::Ally_Occupied;
            team = Team::Ally;
            dir = Direction::Right;
            break;
        case PieceType::Enemy1:
            cx = 5; cz = 1;
            to = TileOccupancy::Enemy_Occupied;
            team = Team::Enemy;
            dir = Direction::Left;
            break;
        case PieceType::Enemy2:
            cx = 5; cz = 3;
            to = TileOccupancy::Enemy_Occupied;
            team = Team::Enemy;
            dir = Direction::Left;
            break;
        }
        ChangeTileTO(cx, cz, to);
        
        auto [px, pz] = CellToWorld(cx, cz);
        auto pPiece = m_objectManager->CreateObject<Piece>(L"Piece", XMFLOAT3(px, m_wy, pz));
        //auto pPiece = m_objectManager->CreateObjectFromFile<Piece>(L"Drill", XMFLOAT3(4, 2, 0), L"../Assets/fbx/Drill/Drill.fbx");
        pPiece->SetScale({ 0.5f, 0.5f, 0.5f });
        pPiece->SetDir(dir);
        m_pieces.emplace(i, PieceInfo{ cx, cz, pPiece->GetID(), team });
    }
   
}

