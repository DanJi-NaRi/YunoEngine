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

    // 무기 2개 기준 초기 카드 생성
    std::vector<yuno::net::packets::CardSpawnInfo>
        ServerCardDealer::MakeInitialDeck(
            uint32_t weapon1,
            uint32_t weapon2)
    {
        std::vector<yuno::net::packets::CardSpawnInfo> result;

        auto c1 = CollectCardsByWeapon(weapon1);
        auto c2 = CollectCardsByWeapon(weapon2);

        // 무기 1
        for (size_t i = 0; i < 6 && i < c1.size(); ++i)
        {
            uint32_t dataID = c1[i];
            uint32_t runtimeID = m_runtime.CreateCard(dataID);

            result.push_back(yuno::net::packets::CardSpawnInfo{
                runtimeID,
                dataID
                });
        }

        // 무기 2
        for (size_t i = 0; i < 6 && i < c2.size(); ++i)
        {
            uint32_t dataID = c2[i];
            uint32_t runtimeID = m_runtime.CreateCard(dataID);

            result.push_back(yuno::net::packets::CardSpawnInfo{
                runtimeID,
                dataID
                });
        }

        return result;
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
}
