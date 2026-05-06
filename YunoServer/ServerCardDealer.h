#pragma once
#include <vector>
#include <cstdint>

#include "S2C_CardPackets.h"

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

        std::vector<uint32_t> CollectCardsByWeapon(uint32_t weaponId);

        uint32_t CreateRuntimeCard(uint32_t dataID);
    private:
        ServerCardManager& m_cardDB;
        ServerCardRuntime& m_runtime;
    };
}
