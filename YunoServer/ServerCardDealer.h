#pragma once
#include <vector>
#include <cstdint>

#include "S2C_StartCardList.h"

// CSV 파싱한 카드 데이터들 존재하는 곳

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

        std::vector<yuno::net::packets::CardSpawnInfo> MakeInitialDeck(
            uint32_t weapon1,
            uint32_t weapon2);

    private:
        std::vector<uint32_t> CollectCardsByWeapon(uint32_t weaponId);

        ServerCardManager& m_cardDB;
        ServerCardRuntime& m_runtime;
    };
}
