#include <iostream>
#include <random>

#include "TurnManager.h"
#include "MatchManager.h"
#include "YunoServerNetwork.h"
#include "RoundController.h"

#include "PacketBuilder.h"
#include "S2C_BattlePackets.h"

static bool IsALess(const ResolvedCard& a, const ResolvedCard& b, bool& coinTossUsed)
{
    if (a.speed != b.speed) return a.speed < b.speed;
    if (a.weaponId != b.weaponId) return a.weaponId < b.weaponId;

    // 완전 동일 => 코인토스
    coinTossUsed = true;
    return (std::rand() & 1) == 0; // true면 a가 먼저
}

namespace yuno::server
{
    TurnManager::TurnManager(
        MatchManager& match,
        YunoServerNetwork& network,
        ServerCardRuntime& runtime,
        ServerCardManager& cardDB)
        : m_match(match)
        , m_network(network)
        , m_runtime(runtime)
        , m_cardDB(cardDB)
    {
    }

    void TurnManager::SubmitTurn(
        uint64_t sessionId,
        const std::vector<uint32_t>& runtimeIDs)
    {
        int idx = m_match.FindSlotBySessionId(sessionId);
        if (idx < 0) return;

        m_turnCards[idx] = runtimeIDs;
        m_submitted[idx] = true;

        std::cout << "[Server] Player " << idx
            << " submitted " << runtimeIDs.size()
            << " cards\n";

        TryResolveTurn();
    }

    void TurnManager::BroadcastTestBattleResult(
        const ResolvedCard& c)
    {
        using namespace yuno::net::packets;

        S2C_BattleResult pkt;
        pkt.runtimeCardId = c.runtimeId;
        pkt.ownerSlot = static_cast<uint8_t>(c.ownerSlot);

        //  테스트용 하드코딩 결과
        pkt.deltas.push_back({
            0,      // slot
            0,       //unit   
            -10,    // hpDelta
            0,      // staminaDelta
            1,      // xDelta
            0       // yDelta
            });


        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_BattleResult,
            [&](yuno::net::ByteWriter& w)
            {
                pkt.Serialize(w);
            });

        //  두 클라에게 동일한 결과
        m_network.Broadcast(std::move(bytes));

        std::cout
            << "[Server] Broadcast BattleResult runtime="
            << c.runtimeId << "\n";
    }


    void TurnManager::TryResolveTurn()
    {
        if (!(m_submitted[0] && m_submitted[1]))
            return;

        std::cout << "[Server] Both players submitted turn\n";

        std::vector<ResolvedCard> slotCards[2];
        std::vector<ResolvedCard> finalOrder;
        std::random_device rd;
        std::mt19937 rng(rd());

        for (int slot = 0; slot < 2; ++slot)
        {
            int localIndex = 0;

            for (uint32_t runtimeId : m_turnCards[slot])
            {
                uint32_t dataId = m_runtime.GetDataID(runtimeId);
                const CardData& card = m_cardDB.GetCardData(dataId);

                slotCards[slot].push_back({
                    runtimeId,
                    dataId,
                    card.m_allowedUnits,
                    card.m_speed,
                    slot,
                    localIndex++
                    });

                std::cout
                    << "[Server] Received Card | slot=" << slot
                    << " local=" << localIndex - 1
                    << " runtimeId=" << runtimeId
                    << " dataId=" << dataId
                    << " name=" << card.m_name
                    << " speed=" << card.m_speed
                    << " weapon=" << card.m_allowedUnits
                    << "\n";
            }
        }

        size_t maxCount =
            std::max(slotCards[0].size(), slotCards[1].size());

        for (size_t i = 0; i < maxCount; ++i)
        {
            bool hasA = i < slotCards[0].size();
            bool hasB = i < slotCards[1].size();

            if (hasA && hasB)
            {
                bool coinTossUsed = false;
                bool aFirst = IsALess(slotCards[0][i], slotCards[1][i], coinTossUsed);

                if (coinTossUsed)
                {
                    std::cout << "[Server] COIN TOSS EVENT "
                        << "A(runtime=" << slotCards[0][i].runtimeId << ") vs "
                        << "B(runtime=" << slotCards[1][i].runtimeId << ")\n";
                }

                if (aFirst)
                {
                    finalOrder.push_back(slotCards[0][i]);
                    finalOrder.push_back(slotCards[1][i]);
                }
                else
                {
                    finalOrder.push_back(slotCards[1][i]);
                    finalOrder.push_back(slotCards[0][i]);
                }
            }
            else if (hasA)
            {
                finalOrder.push_back(slotCards[0][i]);
            }
            else if (hasB)
            {
                finalOrder.push_back(slotCards[1][i]);
            }
        }
        //for (const auto& c : finalOrder)
        //{
        //    //ExecuteCard(c); //  여기서 행동 계산 호출
        //}

        //디버깅용
        for (const auto& c : finalOrder)
        {
            const CardData& card = m_cardDB.GetCardData(c.dataId);

            std::cout
                << "[EXEC] slot=" << c.ownerSlot
                << " local=" << c.localIndex
                << " runtime=" << c.runtimeId
                << " data=" << c.dataId
                << " name=" << card.m_name
                << " speed=" << card.m_speed
                << " weapon=" << card.m_allowedUnits
                << "\n";
        }
        // 테스트용 결과 보내기
        if (!finalOrder.empty())
        {
            BroadcastTestBattleResult(finalOrder[0]);
        }
        // TODO:
        // - 카드 속도 기준 정렬 V 완료
        // - 행동 실행
        // - 결과 패킷 생성

        ClearTurn();
    }

    void TurnManager::ClearTurn()
    {
        m_turnCards[0].clear();
        m_turnCards[1].clear();
        m_submitted[0] = false;
        m_submitted[1] = false;
    }
}
