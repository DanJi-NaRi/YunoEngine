#pragma once
#include <unordered_map>

namespace yuno::server
{
    class MatchManager;
    class ServerCardDealer;
    class YunoServerNetwork;

    class RoundController
    {
    public:
        RoundController(
            MatchManager& match,
            ServerCardDealer& dealer,
            YunoServerNetwork& network);

        void TryStartRound();

        uint8_t GetUnitId(int slotIdx, int localUnitIdx) const;

    private:
        void SendCountDown();
        void SendInitialCards();
        void SendRoundStart();

        MatchManager& m_match;
        ServerCardDealer& m_cardDealer;
        YunoServerNetwork& m_network;

        bool m_roundStarted = false;
        std::unordered_map<uint32_t, uint8_t> m_unitIdMap;
    };
}
