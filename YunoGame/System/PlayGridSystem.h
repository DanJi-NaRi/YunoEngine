#pragma once
#include "GridSystem.h"
#include "PieceHelper.h"
#include "TileHelper.h"

class PieceQ;

class PlayGridSystem : public GridSystem
{
public:
    PlayGridSystem(ObjectManager* objmng);                                      // 씬에서 objmanager 받기
    virtual ~PlayGridSystem();

    void Init(int row, int column, float cellSizeX, float cellSizeZ) override;  
    void CreateObject(float x, float y, float z) override;                      
    void Update(float dt) override;
    
private:
    void ClearTileState();
 
private:
    std::vector<TileState> m_tiles;                         // 타일 상태
    std::vector<uint32_t> m_tilesIDs;                       // 타일 오브젝트 아이디
    std::unordered_map<PieceType, PieceInfo> m_pieces;      // 기물 정보

    std::unique_ptr<PieceQ> m_pieceQ;
};

