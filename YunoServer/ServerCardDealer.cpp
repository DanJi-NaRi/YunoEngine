#include <algorithm>

#include "ServerCardDealer.h"
#include "ServerCardManager.h"
#include "ServerCardRuntime.h"

namespace yuno::server
{
    ServerCardDealer::ServerCardDealer(
        ServerCardManager& cardDB,
        ServerCardRuntime& runtime)
        : m_cardDB(cardDB)
        , m_runtime(runtime)
    {
    }


    // CSV 기준: 무기 -> 카드 DataID 수집
    std::vector<uint32_t>
        ServerCardDealer::CollectCardsByWeapon(uint32_t weaponId)
    {
        std::vector<uint32_t> result;

        for (const auto& [cardId, card] : m_cardDB.GetAllCards())
        {
            if (card.m_allowedUnits == weaponId)
            {
                result.push_back(card.m_cardID);
            }
        }

        // 정렬 기준 (ID 오름차순 = CSV 순서)
        std::sort(result.begin(), result.end());
        return result;
    }

    uint32_t ServerCardDealer::CreateRuntimeCard(uint32_t dataID)
    {
        return m_runtime.CreateCard(dataID);
    }
}
