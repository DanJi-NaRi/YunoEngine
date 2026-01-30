#pragma once
#include <vector>
#include <cstdint>

struct ResolvedCard
{
    uint32_t runtimeId;
    uint32_t dataId;

    uint32_t weaponId;
    int speed;

    int ownerSlot; // 0 or 1 (누가 냈는지)
    //int unitLocalIndex; // 0 or 1 (몇번 유닛인지)
    int localIndex; // 0~3까지에 카드 순서

};

namespace yuno::server
{
    class MatchManager;
    class YunoServerNetwork;
    class ServerCardManager;
    class ServerCardRuntime;
    class RoundController;

    class TurnManager
    {
    public:
        TurnManager(
            MatchManager& match,
            YunoServerNetwork& network,
            ServerCardRuntime& runtime,
            ServerCardManager& cardDB);

        void SubmitTurn(
            uint64_t sessionId,
            const std::vector<uint32_t>& runtimeCardIds);

    private:
        void TryResolveTurn();
        void ClearTurn();
        void BroadcastTestBattleResult(const ResolvedCard& card);

        MatchManager& m_match;
        YunoServerNetwork& m_network;
        ServerCardRuntime& m_runtime;
        ServerCardManager& m_cardDB;

        std::vector<uint32_t> m_turnCards[2];
        bool m_submitted[2] = { false, false };
    };
}
