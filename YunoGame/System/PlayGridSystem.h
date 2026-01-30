#pragma once
#include "GridSystem.h"
#include "PieceHelper.h"
#include "TileHelper.h"

class PlayGridQ;


enum class NG_P : int   // NowGrid_PlayGrid
{
    One,
    Max
};

class PlayGridSystem : public GridSystem<ObjectManager, Unit>
{
public:
    PlayGridSystem(ObjectManager* objmng);                   // 씬에서 objmanager 받기
    virtual ~PlayGridSystem();

    void CreateObject(float x, float y, float z) override;                      
    void Update(float dt) override;
    
private:
    void Init();
    void CreateTileAndPiece(float x, float y, float z);
    void CheckMyQ();

private:
    void MoveEvent(const GamePiece& pieceType, int cx, int cz);
    
    bool CheckExisting(const GamePiece pieceType);
    void CheckHealth(PieceInfo& pieceInfo);


private:
    void ChangeTileTO(int cx, int cz, const TileOccupy to);
    const TileOccupy GetTileTO(int cx, int cz);

    Direction GetDir(float oldcx, float oldcz, float cx, float cz);

    Direction GetCollisionDir(float oldcx, float oldcz, float cx, float cz);
    F2 GetCollisionPos(Direction dir, Direction pieceDir, int cx, int cz);

    std::wstring GetWeaponFileName(int weaponID);           // 테스트용

private:
    void ClearTileState();
 
private:
    float m_wy;                                             // 기물의 world y

    NG_P m_nowG;

    std::vector<TileState> m_tiles;                         // 타일 상태
    std::vector<uint32_t> m_tilesIDs;                       // 타일 오브젝트 아이디
    std::unordered_map<GamePiece, PieceInfo> m_pieces;      // 기물 정보

    std::unique_ptr<PlayGridQ> m_playQ;

    // grid를 여러 개 운용한다면
    //std::unordered_map<NG_P, std::vector<TileState>> m_tiles;    // 타일 상태
    //std::unordered_map<NG_P, std::vector<uint32_t>> m_tilesIDs;  // 타일 오브젝트 아이디
};

