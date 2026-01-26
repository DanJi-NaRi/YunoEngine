#pragma once
#include "GridSystem.h"
#include "PieceHelper.h"
#include "TileHelper.h"

class PieceQ;
class Piece;

class PlayGridSystem : public GridSystem
{
public:
    PlayGridSystem(ObjectManager* objmng);                   // 씬에서 objmanager 받기
    virtual ~PlayGridSystem();

    void Init(int row, int column, float cellSizeX, float cellSizeZ) override;  
    void CreateObject(float x, float y, float z) override;                      
    void Update(float dt) override;
    
private:
    void MoveEvent(const GamePiece& pieceType, int cx, int cz);

private:
    void ChangeTileTO(int cx, int cz, const TileOccupy to);
    const TileOccupy GetTileTO(int cx, int cz);
    Direction GetDir(float oldcx, float oldcz, float cx, float cz);
    Direction GetConverseDir(Direction dir);                // 매개변수에 Direction::Same이 절대 들어가면 안됨

    Direction GetCollisionDir(float oldcx, float oldcz, float cx, float cz);
    F2 GetCollisionEnterPos(Direction dir, Direction pieceDir, int cx, int cz);
    F2 GetCollisionExistPos(Direction dir, Direction pieceDir, int cx, int cz);

private:
    void ClearTileState();
 
private:
    float m_wy;

    std::vector<TileState> m_tiles;                         // 타일 상태
    std::vector<uint32_t> m_tilesIDs;                       // 타일 오브젝트 아이디
    std::unordered_map<GamePiece, PieceInfo> m_pieces;      // 기물 정보

    std::unique_ptr<PieceQ> m_pieceQ;
};

