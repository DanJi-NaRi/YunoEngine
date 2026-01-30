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
    std::vector<uint32_t>
        ServerCardDealer::MakeInitialDeck(
            uint32_t weapon1,
            uint32_t weapon2)
    {
        std::vector<uint32_t> result;

        auto c1 = CollectCardsByWeapon(weapon1);
        auto c2 = CollectCardsByWeapon(weapon2);

        // 앞 6장만 사용
        for (size_t i = 0; i < 6 && i < c1.size(); ++i)
            result.push_back(m_runtime.CreateCard(c1[i]));

        for (size_t i = 0; i < 6 && i < c2.size(); ++i)
            result.push_back(m_runtime.CreateCard(c2[i]));

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
