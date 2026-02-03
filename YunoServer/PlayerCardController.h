#pragma once

namespace yuno::server
{
    class PlayerCardController
    {
    public:
        void InitPlayerCards(PlayerBattleState& player);
        void DrawCards(PlayerBattleState& player, int count);
        bool SelectCard(PlayerBattleState& player, uint32_t runtimeID);
        bool UseCard(PlayerBattleState& player, uint32_t runtimeID);

    private:
        //ServerCardRuntime& m_runtime;
        //std::mt19937 m_rng{ std::random_device{}() };
    };
}


