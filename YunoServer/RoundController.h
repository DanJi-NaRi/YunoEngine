#pragma once
#include <unordered_map>

namespace yuno::server
{
    class MatchManager;
    class ServerCardDealer;
    class YunoServerNetwork;

    // MK 추가
    struct UnitState
    {
        uint8_t slotID = 0;      // 몇번째 유닛인지
        uint8_t WeaponID = 0;    // 무기 아이디
        uint8_t hp = 0;          // 체력
        uint8_t tileID = 0;      // 현재 좌표
    };
    
    struct PlayerUnits
    {
        uint8_t PID = 0;         // 플레이어 아이디
        uint8_t stamina = 0;     // 스태미나
        UnitState unit1;
        UnitState unit2;
    };

    class RoundController
    {
    public:
        RoundController(
            MatchManager& match,
            ServerCardDealer& dealer,
            YunoServerNetwork& network);

        void TryStartRound();

        uint8_t GetUnitId(int slotIdx, int localUnitIdx) const;

        // MK 추가
        PlayerUnits& GetPlayerUnitState(uint8_t PID);

    private:
        void SendCountDown();
        void SendInitialCards();
        void SendRoundStart();

        MatchManager& m_match;
        ServerCardDealer& m_cardDealer;
        YunoServerNetwork& m_network;

        bool m_roundStarted = false;
        std::unordered_map<uint32_t, uint8_t> m_unitIdMap;

        // MK 추가
        std::unordered_map<uint8_t, PlayerUnits> allPlayerUnits; // key: player id, value: player 별 유닛 상태값
    };
}
