#pragma once
#include <unordered_map>

namespace yuno::server
{
    class MatchManager;
    class ServerCardDealer;
    class YunoServerNetwork;
    class PlayerCardController;

    class RoundController
    {
    public:
        RoundController(
            MatchManager& match,
            ServerCardDealer& dealer,
            YunoServerNetwork& network,
            PlayerCardController& mcardController);

        void TryStartRound();
        void EndTurn();

    private:
        void SendCountDown();
        void SendInitialCards();
        void SendRoundStart();
        void SendDrawCandidates();

        void StartTurn();

        MatchManager& m_match;
        ServerCardDealer& m_cardDealer;
        YunoServerNetwork& m_network;
        PlayerCardController& m_cardController;

        bool m_roundStarted = false;
        std::unordered_map<uint32_t, uint8_t> m_unitIdMap;
    };
}
