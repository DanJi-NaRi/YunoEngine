#pragma once

enum class CardType : uint8_t {
    None,
    Buff,
    Move,
    Attack,
    Utility, // 특수
    Count,
    
};

enum class CardRarity : uint8_t {
    None,
    Common, // 일반 등급
    Unique, // 고유 등급
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
static constexpr PieceMask kAny = 0u; // 카드 모두가 씀 // 0 넣어도 동일
static constexpr PieceMask kNone = 0xFFFFFFFFu; // 카드 안씀

constexpr PieceMask PieceBit(PieceType t) { // n번째 비트만 1인 값을 만듦
    return 1u << static_cast<uint8_t>(t);
}

struct CardData // 카드 정보
{
public:
    int m_cardID;               // 카드 고유 ID
    std::string m_name;         // 카드 이름
    CardRarity m_Rarity;        // 카드 희귀도
    CardType m_type;            // 카드 타입
    /*
    * m_allowedUnits 설명
    // 사용 예시 : card.allowedUnits = PieceBit(PieceType::Blaster) | UnitBit(PieceType::Cleaver);
    // 기본값 0 : 모든 기물이 사용 가능
    */
    PieceMask m_allowedUnits;   // 카드 사용 가능 기물  

    int m_speed;                // 카드 속도
    int m_useCnt;               // 턴 당 사용 가능 횟수
    int m_destroyCnt;           // 카드 소멸 횟수
    int m_cost;                 // 소모 스태미나 비용
    uint32_t m_rangeId;         // 카드 범위
    int m_control;
    
    //std::string range // 기획에는 이렇게 써있지만 string??
    //std::pair<int,int> range; // 3 * 3 뭐 이런식으로?
    //int range;

    int m_effectId;             // 사용 시 발동할 Effect ID
    int m_soundId;              // 사용 시 발동할 Sound ID
    std::string m_explainText;  // 카드 설명 텍스트
    

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
    int m_moveX;
    int m_moveY;
};

struct CardEffectData
{
    int m_damage;

    int m_giveDamageBonus;
    int m_takeDamageReduce;
    int m_takeDamageIncrease;

    int m_staminaRecover;
};
