#pragma once
#include "UnitGridSystem.h"
#include "PieceHelper.h"
#include "TileHelper.h"
#include "BattlePackets.h"

class UnitTile;
class PlayGridQ;
enum class CardType : uint8_t;
struct Wdata;
struct CardEffectData;
struct RangeOffset;

enum class NG_P : int   // NowGrid_PlayGrid
{
    One,
    Max
};

enum class HitMove : uint8_t
{
    None,
    Move
};

enum class AttackPhase : uint8_t
{
    None,
    Alaram,
    Attack,
    Hit,
    Over
};
inline void operator++(AttackPhase& a) { a = (AttackPhase)((uint8_t)a + 1); }

struct AttackSequence
{
    GamePiece attacker = GamePiece::None;   // 공격 기물
    Direction dir = Direction::Same;
    std::vector<int> tileIDs;               // 피격 타일들
    std::vector<GamePiece> hitPieces;       // 피격 기물들
    AttackPhase attackPhase = AttackPhase::None;
    bool phaseStarted = false;

    float elapsed = 0.f;                    // 누적된 시간

    int m_flashCount = 0;                   // 번쩍이는 횟수
    float m_flashInterval = 0.f;            // 한번 번쩍일 때 걸리는 시간
    Float4 m_alarmColor{ 1, 1, 1, 1 };      // 아군 여부에 따라 달라짐(파/빨)
    float m_alarmDuration = 0.f;            // 타일 알람 이펙트 시간

    Float4 m_attackColor{ 1, 0.84f, 0, 1 }; // 금색
    float m_attackDuration = 0.f;           // 공격 애니메이션 시간

    Float4 m_tileEffectColor{ 0, 0, 0, 1 }; // 검정색

    Float4 m_hitColor{ 0, 0, 0, 1 };        // 검정색
    float m_hitDuration = 0.f;              // 피격 애니메이션 시간

};

enum class UtilityPhase : uint8_t
{
    None,
    Move,
    AttackAndMove,
    Buff,
    Over
};
inline void operator++(UtilityPhase& a) { a = (UtilityPhase)((uint8_t)a + 1); }

struct MoveInfo
{
    Dirty_US dirty = Dirty_US::None;
    const std::array<UnitState, 4> snapshot;
    int mainUnit = -1;
    Direction dir = Direction::None;
};

struct UtilitySequence
{
    bool phaseStarted = false;
    UtilityPhase utilityPhase = UtilityPhase::None;

    float elapsed = 0.f;                    // 누적된 시간

    GamePiece playPiece = GamePiece::None;

    // move 시행 시
    const MoveInfo* playerMove = nullptr;
    float m_moveDuration = 0.f;

    // attack&move 시행 시
    std::vector<MoveInfo*> hittersMove;
    HitMove hitMove = HitMove::None;
    float m_attackAndMoveDuration = 0.f;

    // buff 시행 시
    const CardEffectData* buffData = nullptr;
    float m_buffDuration = 0.f;
};

struct ObstacleTile
{
    ObstacleType obstacleID = ObstacleType::None;
    std::vector<uint8_t> tileIDs;
};

enum class ObstaclePhase : uint8_t
{
    None,
    Trigger,
    Warning,
    Over
};

struct ObstacleSequence
{
    bool phaseStarted = false;
    ObstaclePhase obstaclePhase = ObstaclePhase::None;
    float elapsed = 0.f;

    // 장애물 발동 ObstaclePhase::Trigger
    float m_triggerDuration = 0.f;
    Float4 hitColor{ 1.f, 1.f, 1.f, 1.f };
    int hitFlashCount = 0;
    float hitFlashInterval = 0.f;
    std::vector<GamePiece> hitPieces;               // 피격 기물들
    ObstacleType attackType = ObstacleType::None;   // 발동 장애물 종류
    std::vector<uint8_t> hitTileIDs;                // 피격 타일들

    // 장애물 경고 ObstaclePhase::Warning
    float m_warningDuration = 0.f;
    Float4 warnColor{ 1.f, 1.f, 1.f, 1.f };
    int warnFlashCount = 0;
    float warnFlashInterval = 0.f;
    ObstacleType warningType = ObstacleType::None;  // 경고 장애물 종류
    std::vector<uint8_t> warningTileIDs;            // 다음 턴 피격될 타일들
};

class EffectManager;
class PlayGridSystem : public UnitGridSystem
{
public:
    PlayGridSystem(ObjectManager* objmng, EffectManager* effectmng);                   // 씬에서 objmanager 받기
    virtual ~PlayGridSystem();

    void CreateObject(float x, float y, float z) override;                      
    void Update(float dt) override;
    
private:
    void Init();
    void CreateTileAndPiece(float x, float y, float z);
    void CreatePiece(const Wdata& wData);

    void InitRound();

    void CheckMyQ();
    void CheckPacket(float dt);
    void CheckOver();
    void ReflectWeaponData();

    void UpdateSequence(float dt);
    void UpdateAttackSequence(float dt);
    void UpdateUtilitySequence(float dt);
    void UpdateObstacleSequence(float dt);

private:
    void ApplyActionOrder(const std::vector<std::array<UnitState, 4>>& order, int mainUnit, uint32_t runCardID, Direction dir);
    bool ApplyBuffChanges(int mainUnit, const CardEffectData*& buffData);
    bool ApplyMoveChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, Direction dir);
    bool ApplyAttackChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit, const std::vector<RangeOffset>& ranges, Direction dir);
    bool ApplyUtilityChanges(Dirty_US dirty, const std::array<UnitState, 4>& newUnitStates, int mainUnit,
        const std::vector<RangeOffset>& ranges, Direction dir, const CardEffectData*& buffData, int snapNum);
    void ApplyObstacleResult(const ObstacleResult& obstacle);   // 장애물 패킷 적용하는 함수

    void MoveEvent(const GamePiece& pieceType, Int2 oldcell, Int2 newcell, Direction moveDir,
        bool isCollided = false, bool isEnemy = false);
    bool BuffEvent(const GamePiece& pieceType, const CardEffectData*& buffData);
    
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

    bool CheckNotDying(const GamePiece pieceType);

    std::wstring GetWeaponFileName(int weaponID);           // 테스트용
    std::wstring GetTileFileName(int tile);
    void SetTileInitState(UnitTile*& pTile, int floornum);

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
    // 시간 관련
    float moveDuration = 2.5f;      // 이동
    float buffDuration = 2.f;       // 버프
    float attackDuration = 3.f;     // 공격

        // 공격
    int tileFlashCount = 3;
    float tileFlashInterval = 0.3f;
    float tileFlashDuration = tileFlashCount * tileFlashInterval;
    float attackRotDuration = 0.25f;
    float offsetDuration = 0.25f;
    float hitDuration = tileFlashDuration;    // 미정. 현재 tileFlashDuration과 동일.
    float chakramAttackAnimDuration = 2.54f;  // ..ㅎㅎ

    float attackAndMoveDuration = attackDuration + tileFlashDuration + hitDuration;


        // 장애물
    int warnFlashCount = 5;
    float warnFlashInterval = 1.f;
    int hitFlashCount = 5;
    float hitFlashInterval = 1.f;

    float triggerDuration = 5.f;
    float warnDuration = 5.f;

        // 기물 디졸브
    float appearDisolveDuration = 1.f;
    float disappearDisolveDuration = 1.25f;
    
        // 패킷. 일단 하드코딩..서버랑 맞춰야해ㅠㅠㅠ
    float pktOffset = disappearDisolveDuration;
    float attackPktTime = attackAndMoveDuration + pktOffset;
    float utilityPktTime = moveDuration + attackAndMoveDuration + buffDuration + pktOffset;
    float obstaclePktDuration = triggerDuration + warnDuration + pktOffset;

    
    // 공격 처리
    bool m_attackActive = false;
    AttackSequence m_attackSequence;

    // 특수 카드 처리
    bool m_utilityActive = false;
    UtilitySequence m_utilitySequence;

    // 장애물 처리
    bool m_obstacleActive = false;
    ObstacleSequence m_obstacleSequence;

    // 패킷 관련 변수
    int m_pID = 0;
    std::array<UnitState, 4> m_UnitStates;
    std::array<uint8_t, 4> m_weaponIDs;
    ObstacleTile m_obstacleTile;            // 장애물 '발동' 타일 정보

    bool isRoundOver = false;
    bool isProcessing = false;
    float m_pktTime = 0;
    float m_currTime = 0;

    // grid를 여러 개 운용한다면
    //std::unordered_map<NG_P, std::vector<TileState>> m_tiles;    // 타일 상태
    //std::unordered_map<NG_P, std::vector<uint32_t>> m_tilesIDs;  // 타일 오브젝트 아이디
    private:
        EffectManager* m_effectManager = nullptr;
};
