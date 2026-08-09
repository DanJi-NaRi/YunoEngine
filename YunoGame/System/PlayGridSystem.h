#pragma once
#include "UnitGridSystem.h"
#include "PieceHelper.h"
#include "TileHelper.h"
#include "BattlePackets.h"
#include "../StateMachine.h"
#include "AttackStates.h"
#include "UtilityStates.h"
#include "ObstacleStates.h"

class UnitTile;
class UnitPiece;
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

// 상태 전환은 PlayGridSystem::ChangeAttackState(AttackPhase)로 수행한다.
struct AttackSequence
{
    GamePiece attacker = GamePiece::None;   // 공격 기물
    Direction dir = Direction::Same;
    std::vector<int> tileIDs;               // 피격 타일들
    std::vector<GamePiece> hitPieces;       // 피격 기물들

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

struct MoveInfo
{
    Dirty_US dirty = Dirty_US::None;
    UnitState prevState{};
    const std::array<UnitState, 4> snapshot;
    int mainUnit = -1;
    Direction dir = Direction::None;
};

// 피격 기물과 그 기물의 넉백/그랩 이동 정보를 한 쌍으로 묶는다.
// 공격 시퀀스의 hitPieces를 따로 참조하지 않으므로 개수 불일치나 수명 문제가 발생하지 않는다.
struct HitterMove
{
    GamePiece piece = GamePiece::None;
    std::unique_ptr<MoveInfo> move;
};

// 상태 전환은 PlayGridSystem::ChangeUtilityState(UtilityPhase)로 수행한다.
struct UtilitySequence
{
    GamePiece playPiece = GamePiece::None;

    // move 시행 시
    std::unique_ptr<const MoveInfo> playerMove = nullptr;
    float m_moveDuration = 0.f;

    // attack&move 시행 시
    std::vector<HitterMove> hittersMove;
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
    // 상태머신 시동 시 시작 단계를 지정하는 용도로만 사용한다. (ChangeObstacleState)
    ObstaclePhase obstaclePhase = ObstaclePhase::None;

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
    void ApplyTransform() override;

public:
    // 시퀀스별 상태 전환. State의 Update 내부에서는 반드시 이 함수들을 통해서만 전환한다.
    void ChangeAttackState(AttackPhase phase);
    void ChangeUtilityState(UtilityPhase phase);
    void ChangeObstacleState(ObstaclePhase phase);

    // State 클래스들이 사용하는 접근자
    AttackSequence& GetAttackSequence() { return m_attackSequence; }
    void SetAttackActive(bool active) { m_attackActive = active; }

    UtilitySequence& GetUtilitySequence() { return m_utilitySequence; }
    void SetUtilityActive(bool active) { m_utilityActive = active; }

    ObstacleSequence& GetObstacleSequence() { return m_obstacleSequence; }
    void SetObstacleActive(bool active) { m_obstacleActive = active; }

    // 공격 시퀀스가 Hit 단계에 진입했음을 알린다. (AttackStates::HitState::Enter에서 호출)
    // Utility가 AttackAndMove에 이미 도착했으면 즉시 넉백/그랩을 적용하고,
    // 아직 도착 전이면 래치에 남겨 두어 AttackAndMove::Enter가 회수하도록 한다.
    void OnAttackHitStarted();
    bool HasAttackHitStarted() const { return m_attackHitStarted; }

    // private 로직 래퍼
    bool ApplyMoveInfo(const MoveInfo* mi)
    {
        return ApplyMoveChanges(mi->dirty, mi->prevState, mi->snapshot, mi->mainUnit, mi->dir);
    }
    bool PlayBuffEvent(const GamePiece& piece, const CardEffectData*& buffData)
    {
        return BuffEvent(piece, buffData);
    }
    bool IsPieceNotDying(const GamePiece piece) { return CheckNotDying(piece); }
    TileState& GetTileState(int tileID) { return m_tiles[tileID]; }
    void SetTileOccupyByID(int tileID, const TileOccupy to)
    {
        auto cell = GetCellByID(tileID);
        ChangeTileTO(cell.x, cell.y, to);
    }

    ObjectManager* GetObjectManager() const { return m_manager; }
    EffectManager* GetEffectManager() const { return m_effectManager; }

    uint32_t GetTileObjectID(int tileID) const { return m_tilesIDs[tileID]; }
    const UnitState& GetUnitState(int unitID) const { return m_UnitStates[unitID]; }
    const PieceInfo* FindPieceInfo(GamePiece piece) const
    {
        auto it = m_pieces.find(piece);
        return (it != m_pieces.end()) ? &it->second : nullptr;
    }
    int GetUnitIDOf(GamePiece gamePiece) { return GetUnitID(gamePiece); }

    float GetAttackRotDuration() const { return attackRotDuration; }
    float GetDisappearDissolveDuration() const { return disappearDisolveDuration; }

private:
    void Init();
    void CreateTileAndPiece(float x, float y, float z);
    void CreatePiece(const Wdata& wData);

    void InitRound();

    void CheckMyQ();
    void CheckPacket(float dt);
    void CheckOver();
    void ReflectWeaponData();
    void ReflectTileData();     // 붕괴 타일 상태를 GameManager로 반영 (Minimap이 소비)

    void UpdateSequence(float dt);

private:
    void ApplyActionOrder(const std::vector<std::array<UnitState, 4>>& order, int mainUnit, uint32_t runCardID, Direction dir);
    bool ApplyBuffChanges(int mainUnit, const CardEffectData*& buffData);
    bool ApplyMoveChanges(Dirty_US dirty, const std::array<UnitState, 4> newUnitStates, int mainUnit, Direction dir);
    bool ApplyMoveChanges(Dirty_US dirty, const UnitState prevUnitState, const std::array<UnitState, 4> newUnitStates, int mainUnit, Direction dir);
    bool ApplyAttackChanges(Dirty_US dirty, const std::array<UnitState, 4> newUnitStates, int mainUnit, const std::vector<RangeOffset>& ranges, Direction dir);
    bool ApplyUtilityChanges(Dirty_US dirty, const std::array<UnitState, 4> newUnitStates, int mainUnit,
        const std::vector<RangeOffset>& ranges, Direction dir, const int controllId, const CardEffectData*& buffData, int snapNum);
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
    bool m_delayFlag = true; //YDM
    // 기물 및 타일 관련 변수
    float m_wy;                                             // 기물의 world y

    NG_P m_nowG;

    std::vector<TileState> m_tiles;                         // 타일 상태
    std::vector<uint32_t> m_tilesIDs;                       // 타일 오브젝트 아이디
    std::unordered_map<GamePiece, PieceInfo> m_pieces;      // 기물 정보

    std::unique_ptr<PlayGridQ> m_playQ;
    std::unordered_map<GamePiece, UnitPiece*> m_units;

private:
    // 시간 관련
    float moveDuration = 3.f;      // 이동
    float buffDuration = 2.f;       // 버프
    float attackDuration = 3.f;     // 공격

        // 공격
    int tileFlashCount = 3;
    float tileFlashInterval = 0.3f;
    float tileFlashDuration = tileFlashCount * tileFlashInterval;
    float attackRotDuration = 0.25f;
    float hitDuration = 2;
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
    bool m_attackHitStarted = false;    // 이번 공격 시퀀스가 Hit 단계에 진입했는가 (Utility 통지용 래치)

    // 공격 상태머신
    StateMachine<PlayGridSystem> m_attackSM{ this };
    AttackStates::AlarmState m_attackAlarmState;
    AttackStates::AttackState m_attackAttackState;
    AttackStates::HitState m_attackHitState;
    AttackStates::OverState m_attackOverState;

    // 특수 카드 상태머신
    StateMachine<PlayGridSystem> m_utilitySM{ this };
    UtilityStates::MoveState m_utilityMoveState;
    UtilityStates::AttackAndMoveState m_utilityAttackAndMoveState;
    UtilityStates::BuffState m_utilityBuffState;
    UtilityStates::OverState m_utilityOverState;

    // 장애물 상태머신
    StateMachine<PlayGridSystem> m_obstacleSM{ this };
    ObstacleStates::TriggerState m_obstacleTriggerState;
    ObstacleStates::WarningState m_obstacleWarningState;
    ObstacleStates::OverState m_obstacleOverState;

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
    bool m_firstDelay = false;
    float m_pktTime = 0;
    float m_currTime = 0;

    // grid를 여러 개 운용한다면
    //std::unordered_map<NG_P, std::vector<TileState>> m_tiles;    // 타일 상태
    //std::unordered_map<NG_P, std::vector<uint32_t>> m_tilesIDs;  // 타일 오브젝트 아이디
    private:
        EffectManager* m_effectManager = nullptr;
};
