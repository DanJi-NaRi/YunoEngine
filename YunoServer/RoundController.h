#pragma once
#include <array>
#include <chrono>
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
        bool CanSelectBonusCard(int playerIdx) const;
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
        void AutoSelectBonusCard(int playerIdx);

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
        std::array<uint32_t, 2> m_selectedBonusRuntimeIds{};
        bool m_waitingCardSelection = false;
        std::chrono::steady_clock::time_point m_cardSelectionDeadline{};

        // The server starts this clock before clients finish battle playback.
        // Keep a generous fail-safe; the client auto-selects 15 seconds after its UI opens.
        static constexpr std::chrono::seconds kCardSelectionTimeout{ 90 };
        std::unordered_map<uint32_t, uint8_t> m_unitIdMap;
    };
}
