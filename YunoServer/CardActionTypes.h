#pragma once

enum class ActionType
{
    Move,        // 위치 변화
    Attack,     // 피해 계산
    Buff,        // 스탯 증가
    Debuff,     // 스탯 감소 / 상태이상
    Heal,        // 스태미나 회복
    Control,    // 그랩 / 넉백
};

enum class EffectType
{
    Damage,
    Knockback,
    Grab,
    DamageBuff,
    DamageReduce,
    StaminaRecover,
    PositionShift,
};
