#pragma once

struct UnitState
{
    uint32_t unitId;     // 선택한 유닛(무기/직업 등)
    PieceType type;

    int x = 0;
    int y = 0;

    int hp = 0;
    int stamina = 0;

    // 버프/디버프 상태도 나중에 여기
};

struct PlayerBattleState
{
    uint32_t sessionId = 0;
    uint32_t userId = 0;

    UnitState unit1;
    UnitState unit2;
};

struct BattleState
{
    PlayerBattleState players[2];
    int turnNumber = 0;
};
