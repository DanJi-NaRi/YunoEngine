#pragma once

namespace yuno::server
{
    struct BuffState
    {
        int nextDamageBonus = 0;
        int nextDamageReduce = 0;
        int nextDamageIncrease = 0;
    };

    struct UnitState
    {
        uint8_t slotID = 0;             // 몇번째 유닛인지 
        uint8_t WeaponID = 0;           // 선택한 유닛(무기/직업 등)

        int x = 0;                      
        int y = 0;                       //배틀 계산용 좌표
        uint8_t tileID = 0;          // 현재 좌표
        uint8_t hp = 0;                 //체력
        uint8_t stamina = 0;            //스테미나

        BuffState buffstat = {0,0,0};

        // 버프/디버프 상태도 나중에 여기
    };


    struct PlayerBattleState
    {
        uint8_t sessionId = 0;          // 유동값
        uint8_t PID = 0;                // 환경변수값

        UnitState unit1;
        UnitState unit2;

        // 아직 안 뽑힌 카드들 (dataID)
        std::vector<uint32_t> remainingDeck;
        // 현재 손에 들고 있는 카드들
        std::vector<yuno::net::packets::CardSpawnInfo> handCards;
        // 클라에게 제시한 카드들
        std::vector<yuno::net::packets::CardSpawnInfo> drawCandidates;
    };

    struct BattleState
    {
        PlayerBattleState players[2];
        int turnNumber = 0;
    };

    extern BattleState g_battleState;
}

