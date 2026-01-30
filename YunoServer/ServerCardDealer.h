#pragma once
#include <vector>
#include <cstdint>

namespace yuno::server
{
    class ServerCardManager;
    class ServerCardRuntime;

    class ServerCardDealer
    {
    public:
        ServerCardDealer(
            ServerCardManager& cardDB,
            ServerCardRuntime& runtime);

        std::vector<uint32_t> MakeInitialDeck(
            uint32_t weapon1,
            uint32_t weapon2);

    private:
        std::vector<uint32_t> CollectCardsByWeapon(uint32_t weaponId);

        ServerCardManager& m_cardDB;
        ServerCardRuntime& m_runtime;
    };
}
