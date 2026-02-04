#pragma once

#include <vector>
#include <cstdint>

namespace yuno::server
{
    struct PlayerBattleState;
    class ServerCardDealer;
    class ServerCardManager;
    class ServerCardRuntime;

    class PlayerCardController
    {
    public:
        PlayerCardController(
            ServerCardDealer& dealer,
            ServerCardManager& cardDB,
            ServerCardRuntime& runtime);

        void InitPlayerCards(PlayerBattleState& player);
        void DrawCards(PlayerBattleState& player);
        bool SelectCard(PlayerBattleState& player, uint32_t runtimeID);
        bool UseCard(PlayerBattleState& player, uint32_t runtimeID);

    private:
        ServerCardDealer& m_cardDealer;
        ServerCardManager& m_cardDB;
        ServerCardRuntime& m_runtime;
    };
}

