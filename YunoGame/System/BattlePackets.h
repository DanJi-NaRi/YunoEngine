#pragma once

// MK 추가
struct UnitState
{
    uint8_t pId = 0;
    uint8_t slotId = 0;

    uint8_t hp = 0;
    uint8_t stamina = 0;
    uint8_t targetTileID = 0;
    uint8_t isEvent = 0;         // 0이면 충돌x, 1이면 충돌o
};

struct BattleResult
{
    uint32_t runTimeCardID = 0;
    uint8_t pId = 0;    // 0 에러
    uint8_t slotId = 0; // 0 에러
    uint8_t dir = 0;    // 0 에러 , 1234 상하좌우
    uint32_t actionTime = 0;

    std::vector<std::array<UnitState, 4>> order;
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
