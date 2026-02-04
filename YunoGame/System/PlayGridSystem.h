#pragma once
#include "UnitGridSystem.h"
#include "PieceHelper.h"
#include "TileHelper.h"
#include "BattlePackets.h"

class PlayGridQ;
enum class CardType : uint8_t;
struct RangeData;
struct RangeOffset;

enum class NG_P : int   // NowGrid_PlayGrid
{
    One,
    Max
};

enum class AttackPhase : uint8_t
{
    None,
    Alaram,
    Attack,
    TileHit,
    UnitHit,
    Over
};
inline void operator++(AttackPhase& a) { a = (AttackPhase)((uint8_t)a + 1); }

struct AttackSequence
{
    GamePiece attacker = GamePiece::None;
    std::vector<int> tileIDs;
    std::vector<GamePiece> hitPieces;
    AttackPhase attackPhase = AttackPhase::None;
    bool phaseStarted = false;

    float elapsed = 0.f;
    
    int m_flashCount = 0;
    float m_flashInterval = 0.f;

    Float4 m_alarmColor{ 1, 1, 1, 1 };
    float m_alarmDuration = 0.f;

    Float4 m_attackColor{ 1, 0.84f, 0, 1 }; // 금색
    float m_attackDuration = 0.f;

    Float4 m_tileEffectColor{ 0, 0, 0, 1 }; // 검정색
    float m_tileEffectDuration = 0.f;

    Float4 m_hitColor{ 1, 0, 0, 1 };        // 빨간색
    float m_hitDuration = 0.f;
};

class PlayGridSystem : public UnitGridSystem
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
    void CheckPacket(float dt);
    void UpdateAttackSequence(float dt);

private:
    void MoveEvent(const GamePiece& pieceType, Int2 oldcell, Int2 newcell, Direction moveDir,
        bool isCollided = false, bool isEnemy = false, int damage1 = 0, int damage2 = 0);

    void ApplyActionOrder(const std::vector<std::array<UnitState, 4>>& order, int mainUnit, CardType cardType, const RangeData* rangeData, Direction dir);
    void ApplyMoveChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, Direction dir);
    void ApplyAttackChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, const std::vector<RangeOffset>& ranges, Direction dir);
    
private:
    void ChangeTileTO(int cx, int cz, const TileOccupy to);
    const TileOccupy GetTileTO(int cx, int cz);

    Direction Get2Dir(float oldcx, float oldcz, float cx, float cz);            // 좌-우 2방향 체크
    Direction Get8Dir(float oldcx, float oldcz, float cx, float cz);            // 상하좌우 대각선 8방향 체크
    std::pair<Float2, Int2> GetCollisionPos(Direction dir, int cx, int cz);     // 충돌 월드좌표와 셀좌표

    GamePiece GetGamePiece(int pId, int unitId);
    int GetUnitID(int pId, int slotID);
    int GetUnitID(GamePiece gamePiece);
    int GetOtherUnitDamage(const std::array<UnitState, 4>& newUnitStates, int mainUnit);
    const std::vector<int> GetRangeTileIDs(const Int2 unitCell, const std::vector<RangeOffset>& ranges, Direction dir);

    bool CheckExisting(const GamePiece pieceType);
    void CheckHealth(const UnitState& us, PieceInfo& pieceInfo);

    std::wstring GetWeaponFileName(int weaponID);           // 테스트용
    std::wstring GetTileFileName(int tile);

private:
    void ClearTileState();
 
private:
    // 기물 및 타일 관련 변수
    float m_wy;                                             // 기물의 world y

    NG_P m_nowG;

    std::vector<TileState> m_tiles;                         // 타일 상태
    std::vector<uint32_t> m_tilesIDs;                       // 타일 오브젝트 아이디
    std::unordered_map<GamePiece, PieceInfo> m_pieces;      // 기물 정보

    std::unique_ptr<PlayGridQ> m_playQ;

private:
    // 공격 처리
    bool m_attackActive = false;
    AttackSequence m_attackSequence;

    // 패킷 관련 변수
    int m_pID = 0;
    std::array<UnitState, 4> m_UnitStates;

    bool isProcessing = false;
    float m_pktTime = 0;
    float m_currTime = 0;

    // grid를 여러 개 운용한다면
    //std::unordered_map<NG_P, std::vector<TileState>> m_tiles;    // 타일 상태
    //std::unordered_map<NG_P, std::vector<uint32_t>> m_tilesIDs;  // 타일 오브젝트 아이디
};

