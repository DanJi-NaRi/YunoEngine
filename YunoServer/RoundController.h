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
        void Update();
        void OnRoundStartReadyOK(int playerIdx);
        void EndTurn();
        void EndGame();
        void OnPlayerSelectedCard(int playerIdx);
        void ResetMatchState();

        void EndGameByDisconnect(uint8_t winnerPID, uint32_t winnerSessionId);

        
        bool GetRoundStarted() { return m_roundStarted; }
        bool IsMatchLocked() const { return m_matchLocked; }

        bool IsRoundStarted() { return m_roundStarted; }

        void EndRound();
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
        bool m_cardsInitialized = false;

        bool m_matchLocked = false;

        bool m_waitingRoundStartReady = false;
        bool m_roundStartReady[2] = { false, false };

        bool m_cardSelected[2] = { false, false };
        std::unordered_map<uint32_t, uint8_t> m_unitIdMap;
    };
}
