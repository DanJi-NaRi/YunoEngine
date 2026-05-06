#pragma once
#include <vector>
#include <cstdint>

#include "C2S_BattlePackets.h"

struct ResolvedCard
{
    uint32_t runtimeId;
    uint32_t dataId;

    uint32_t weaponId;
    int speed;

    int ownerSlot; // 0 or 1 (누가 냈는지)
    int localIndex; // 0 ~ 3까지에 카드 순서

    Direction dir;
};

namespace yuno::server
{
    class MatchManager;
    class YunoServerNetwork;
    class ServerCardManager;
    class ServerCardRangeManager;
    class ServerCardRuntime;
    class RoundController;
    class ServerObstacleManager;

    class TurnManager
    {
    public:
        TurnManager(
            MatchManager& match,
            YunoServerNetwork& network,
            ServerCardRuntime& runtime,
            ServerCardManager& cardDB,
            ServerCardRangeManager& rangeDB,
            ServerObstacleManager& obstacleDB,
            RoundController& roundController);

        void SubmitTurn(
            uint64_t sessionId,
            const std::vector<CardPlayCommand>& commands);

    private:
        void TryResolveTurn();
        void ClearTurn();
        void BroadcastTestBattleResult(const ResolvedCard& card);
        void NotifyEndFinished();
        void SendObstacleResult();

        MatchManager& m_match;
        YunoServerNetwork& m_network;
        ServerCardRuntime& m_runtime;
        ServerCardManager& m_cardDB;
        ServerCardRangeManager& m_cardRangeDB;
        ServerObstacleManager& m_obstacleDB;
        RoundController& m_roundController;

        std::vector<CardPlayCommand> m_turnCards[2];
        bool m_submitted[2] = { false, false };
    };
}
