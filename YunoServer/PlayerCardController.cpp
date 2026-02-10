#include "PlayerCardController.h"

#include "YunoServerNetwork.h"
#include "ServerCardDealer.h"
#include "ServerCardManager.h"

using namespace yuno::server;

PlayerCardController::PlayerCardController(
    ServerCardDealer& dealer,
    ServerCardManager& cardDB,
    ServerCardRuntime& runtime)
    : m_cardDealer(dealer)
    , m_cardDB(cardDB)
    , m_runtime(runtime)
{
}
//카드 초기화
void PlayerCardController::InitPlayerCards(PlayerBattleState& player)
{
    player.remainingDeck.clear();
    player.handCards.clear();
    player.drawCandidates.clear();

    auto collect = [&](uint8_t weaponID)
        {
            auto cards = m_cardDealer.CollectCardsByWeapon(weaponID);

            for (uint32_t dataID : cards)
            {
                const auto& card = m_cardDB.GetCardData(dataID);

                if (card.m_Rarity == 1)       // 이게 디폴트임
                //if (card.m_Rarity == 1|| card.m_Rarity == 2)         // 이건 테스트임
                {
                    // 초기 카드 -> 즉시 hand
                    uint32_t runtimeID =
                        m_cardDealer.CreateRuntimeCard(dataID);

                    uint8_t unitSlot =
                        (weaponID == player.unit1.WeaponID)
                        ? player.unit1.slotID
                        : player.unit2.slotID;

                    player.handCards.push_back({
                        player.PID,
                        unitSlot,
                        runtimeID,
                        dataID
                        });
                }
                else
                {
                    // 나머지 -> 후보 덱
                    player.remainingDeck.push_back(dataID);
                }
            }
        };

    collect(player.unit1.WeaponID);
    collect(player.unit2.WeaponID);
}

//추가 카드 생성
void PlayerCardController::DrawCards(PlayerBattleState& player)
{
    player.drawCandidates.clear();

    if (player.remainingDeck.size() < 3)
        return;

    for (int i = 0; i < 3; ++i)
    {
        int idx = std::rand() % player.remainingDeck.size();
        uint32_t dataID = player.remainingDeck[idx];

        uint32_t runtimeID =
            m_cardDealer.CreateRuntimeCard(dataID);

        const auto& card = m_cardDB.GetCardData(dataID);

        uint8_t unitSlot =
            (card.m_allowedUnits == player.unit1.WeaponID)
            ? player.unit1.slotID
            : player.unit2.slotID;

        player.drawCandidates.push_back({
            player.PID,
            unitSlot,
            runtimeID,
            dataID
            });
    }
}

//선택 카드 추가
bool PlayerCardController::SelectCard(PlayerBattleState& player, uint32_t runtimeID)
{
    auto it = std::find_if(
        player.drawCandidates.begin(),
        player.drawCandidates.end(),
        [&](const auto& c)
        {
            return c.runtimeID == runtimeID;
        });

    if (it == player.drawCandidates.end())
        return false;

    player.handCards.push_back(*it);

    const auto& card =
        m_cardDB.GetCardData(it->dataID);

    if (card.m_useId == 0)
    {
        // 무제한 카드 -> 후보에서 완전 제거
        player.remainingDeck.erase(
            std::remove(
                player.remainingDeck.begin(),
                player.remainingDeck.end(),
                it->dataID),
            player.remainingDeck.end());
    }

    for (const auto& c : player.drawCandidates)
    {
        if (c.runtimeID != runtimeID)
        {
            m_runtime.RemoveCard(c.runtimeID);
        }
    }

    player.drawCandidates.clear();
    return true;
}

//사용 카드 확인
bool PlayerCardController::UseCard(PlayerBattleState& player, uint32_t runtimeID)
{
    auto it = std::find_if(
        player.handCards.begin(),
        player.handCards.end(),
        [&](const auto& c)
        {
            return c.runtimeID == runtimeID;
        });

    if (it == player.handCards.end())
        return false;

    const auto& card =
        m_cardDB.GetCardData(it->dataID);

    if (card.m_useId != 0)
    {
        // 소모 카드 -> 손패에서 제거
        player.handCards.erase(it);
        m_runtime.RemoveCard(runtimeID);
    }
    return true;
}
