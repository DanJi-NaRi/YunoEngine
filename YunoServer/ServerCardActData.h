#pragma once

enum class RangeType : uint8_t {
    Front,
    Cross,
};

enum CardUtility : uint8_t {
    None,
    Stun,
    Push,
    Grab,
    DamageBuff,
};

struct CardActData
{
public:
    std::pair<int, int> m_actStartPoint;    // 행동 시작 위치
    RangeType m_rangeType;                // 행동 타입
    int m_range;                                      // 행동 범위
    CardUtility m_util;                              // 특수 효과
    int m_utilStat;                                    // 특수 효과 수치
    int m_damage;                                  // 카드 데미지
    Dir m_Dir;                                          // 사용 방향
};
