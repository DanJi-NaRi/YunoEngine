#pragma once

// MK 추가
struct UnitState
{
    uint8_t pId = 0;    // 플레이어
    uint8_t slotId = 0; // 유닛

    uint8_t hp = 0;
    uint8_t stamina = 0;
    uint8_t targetTileID = 0;
    uint8_t isEvent = 0;         // 0이면 충돌x, 1이면 충돌o
    
    uint8_t weaponId = 0;
};

struct BattleResult
{
    uint32_t runTimeCardID = 0;
    uint8_t pId = 0;    // 0 에러
    uint8_t slotId = 0; // 0 에러
    uint8_t dir = 0;    // 0 에러 , 1234 상하좌우
    uint32_t actionTime = 0;
    // 장애물 타일 아이디(배열)
    // 장애물 타입 : 0은 에러. 수평 레이저. 수직 레이저. 붕괴.

    std::vector<std::array<UnitState, 4>> order;
};


enum class ObstacleType : uint8_t
{
    None,
    Vertical_Razer,
    Horizon_Razer,
    Collapse
};


struct ObstacleResult
{
    // 다음 라운드에 발동할 장애물 경고 정보

    ObstacleType obstacleID = ObstacleType::None;

    std::vector<uint8_t> tileIDs;

    // 이전 라운드에서 경고된 장애물이 발동된 결과 스냅샷
    std::array<UnitState, 4> unitState{};
};

enum class Dirty_US : uint8_t
{
    None            = 0,
    pId             = 1 << 0,
    slotId          = 1 << 1,
    hp              = 1 << 2,
    stamina         = 1 << 3,
    targetTileID    = 1 << 4,
    isEvent         = 1 << 5
};

inline Dirty_US operator|(Dirty_US a, Dirty_US b) {
    return (Dirty_US)((uint8_t)a | (uint8_t)b);
}

inline bool HasThis_US(Dirty_US a, Dirty_US b) {
    return(((uint8_t)a & (uint8_t)b) != 0);
}

Dirty_US Diff_US(const UnitState& prev, const UnitState& cur);
