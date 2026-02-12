#pragma once

enum class CardType : uint8_t {
    None,
    Buff,
    Move,
    Attack,
    Utility, // 특수
    Count,

};

enum class PieceType : uint8_t { // 비트마스크로도 활용 가능
    None,
    Blaster, // 1 부터 시작
    Breacher,
    Impactor,
    Chakram,
    Scythe,
    Cleaver,
    Count,

};

enum Dir : uint8_t {
    None,
    North,
    South,
    East,
    West,
    Count,
};

using PieceMask = uint32_t;
static constexpr PieceMask kAny = 0u;                       
static constexpr PieceMask kNone = 0xFFFFFFFFu; 

constexpr PieceMask PieceBit(PieceType t) { 
    return 1u << static_cast<uint8_t>(t);
}

struct CardData // 카드 정보
{
public:
    int m_cardID;                   // 카드 고유 ID
    std::string m_name;         // 카드 이름
    int m_Rarity;       // 카드 희귀도
    CardType m_type;           // 카드 타입

    PieceMask m_allowedUnits;   // 카드 사용 가능 기물

    int m_speed;                    // 카드 속도
    int m_useId;                  // 사용ID
    int m_destroyCnt;            // 카드 소멸 횟수
    int m_cost;                      // 소모 스태미나 비용
    int m_controlId;                  //그랩 및 넉백 같은 효과ID  // 0이 효과 x, 1이 당기기, 2가 밀기
    uint32_t m_rangeId;        // 카드 범위
    int m_actionTime;

    int m_effectId;                     // 사용 시 발동할 Effect ID
    int m_soundId;                    // 사용 시 발동할 Sound ID
    std::string m_explainText;   // 카드 설명 텍스트


public:
    inline bool CanUse(const PieceMask& mask, PieceType type) {
        if (mask == kAny)  return true;
        if (mask == kNone) return false;
        return (mask & PieceBit(type)) != 0;
    }
    inline bool CanUse(const CardData& card, PieceType type) {
        return CanUse(card.m_allowedUnits, type);
    }
};

struct CardMoveData
{
    int m_moveX; //이동량
    int m_moveY;
};

struct CardEffectData
{
    int m_damage; //데미지
    //주는 피해 증가량,    받는 피해 감소량,    받는 피해 증가량
    int m_giveDamageBonus;
    int m_takeDamageReduce;
    int m_takeDamageIncrease;

    int m_staminaRecover;
    int m_pullDistance;
    int m_pushDistance;
};
