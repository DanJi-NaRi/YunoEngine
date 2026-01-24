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
        PieceType& pieceType = cmd.whichPiece;                      // 기물 확인
        PieceInfo& PieceInfo = m_pieces[pieceType];
        Unit* pUnit = m_objectManager->FindObject(PieceInfo.id);   // 기물 아이디 통해 오브젝트 찾음

        switch (cmd.cmdType)
        {
        case CommandType::Move:
        {
            Piece* pPiece = dynamic_cast<Piece*>(pUnit);
            auto [x, z] = cmd.mv;
            auto [wx, wz] = CellToWorld(x, z);
            pPiece->SetTarget({wx, m_y, wz});
            break;
        }
            
        }
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

    m_y = y;
    for (auto i = PieceType::Ally1; i < PieceType::MAX; ++i)
    {
        int cx = 0; int cz = 0;
        switch (i)
        {
        case PieceType::Ally1:
            cx = 1; cz = 1;
            break;
        case PieceType::Ally2:
            cx = 1; cz = 3;
            break;
        case PieceType::Eenemy1:
            cx = 5; cz = 1;
            break;
        case PieceType::Eenemy2:
            cx = 5; cz = 3;
            break;
        }
        
        auto [px, pz] = CellToWorld(cx, cz);
        auto pPiece = m_objectManager->CreateObject<Piece>(L"Piece", XMFLOAT3(px, m_y, pz));
        pPiece->SetScale({ 0.5f, 0.5f, 0.5f });
        m_pieces.emplace(i, PieceInfo{ cx, cz, pPiece->GetID()});
    }
   
}

