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
    DamageBuff, // 이렇게만 설정 후, +-값으로 버프/너프 설정 예정
    // 데미지 감소량이나 이런거 +연산인가 곱연산인가??
    Heal, // 얘도 dmg +-값으로 해야할까?
};

struct CardActData // 카드 행동 데이터
{
public:
    std::pair<int, int> m_actStartPoint; // 행동 시작 위치<XY>
    RangeType m_rangeType;               // 행동 타입
    int m_range;                         // 행동 범위(기본 공격 타입 * n)
    CardUtility m_util;                  // 특수 효과
    int m_utilStat;                      // 특수 효과 수치 (사용하기 나름. 스턴 턴이라든지 버프량이라든지..) // 추후 해당 방식은 바뀔 수 있음
    int m_damage;                        // 카드 데미지
    Dir m_Dir;                           // 사용 방향
};
