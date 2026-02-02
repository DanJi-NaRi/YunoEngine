#include "RoundController.h"

#include "MatchManager.h"
#include "ServerCardDealer.h"
#include "YunoServerNetwork.h"
#include "ServerCardInstance.h"

#include "PacketBuilder.h"
#include "S2C_CountDown.h"
#include "S2C_RoundStart.h"
#include "S2C_StartCardList.h"

namespace yuno::server
{
    RoundController::RoundController(
        MatchManager& match,
        ServerCardDealer& dealer,
        YunoServerNetwork& network)
        : m_match(match)
        , m_cardDealer(dealer)
        , m_network(network)
    {
    }

    void RoundController::TryStartRound()
    {
        const auto& s = m_match.Slots();

        //  라운드 시작 조건 
        if (!(s[0].occupied && s[1].occupied)) return;
        if (!(s[0].ready && s[1].ready)) return;
        if (!(s[0].IsUnitsFilled() && s[1].IsUnitsFilled())) return;
        if (m_roundStarted) return;

        m_roundStarted = true;

        SendCountDown();
        SendInitialCards();
        SendRoundStart();
    }

    // ------------------------------------------------------

    PlayerUnits& RoundController::GetPlayerUnitState(uint8_t PID)
    {
        auto it = allPlayerUnits.find(PID);
        if (it == allPlayerUnits.end())
        {
            assert(false);
        }
        return it->second;
    }

    void RoundController::SendCountDown()
    {
        yuno::net::packets::S2C_CountDown cd{};
        cd.countTime = 3;

        const auto& s = m_match.Slots();
        cd.slot1_UnitId1 = s[0].unitId1;
        cd.slot1_UnitId2 = s[0].unitId2;
        cd.slot2_UnitId1 = s[1].unitId1;
        cd.slot2_UnitId2 = s[1].unitId2;

        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_CountDown,
            [&](yuno::net::ByteWriter& w)
            {
                cd.Serialize(w);
            });

        m_network.Broadcast(std::move(bytes));
    }

    // ------------------------------------------------------

    void RoundController::SendInitialCards()
    {
        const auto& slots = m_match.Slots();

        for (const auto& slot : slots)
        {
            if (!slot.occupied)
                continue;

            auto session = m_network.FindSession(slot.sessionId);
            if (!session)
                continue;

            // 무기 기반 카드 풀 생성 (서버 전용 정보)
            auto cards =
                m_cardDealer.MakeInitialDeck(
                    slot.unitId1,
                    slot.unitId2);

            yuno::net::packets::S2C_TestCardList pkt;
            pkt.cards = std::move(cards);

            auto bytes = yuno::net::PacketBuilder::Build(
                yuno::net::PacketType::S2C_TestCardList,
                [&](yuno::net::ByteWriter& w)
                {
                    pkt.Serialize(w);
                });

            session->Send(std::move(bytes));
        }
    }

    // ------------------------------------------------------

    void RoundController::SendRoundStart()
    {
        const auto& s = m_match.Slots();

        yuno::net::packets::S2C_RoundStart rs{};
        
        // MK 추가
        PlayerUnits player;
        UnitState unit1, unit2;

        // slot0 (P1)
        player.PID = rs.units[0].PID = 1;
        player.unit1.slotID = rs.units[0].slotID = 1;
        player.unit1.WeaponID = rs.units[0].WeaponID = static_cast<std::uint8_t>(s[0].unitId1);
        player.unit1.hp = rs.units[0].hp = 100;
        player.stamina = rs.units[0].stamina = 100;
        player.unit1.tileID = rs.units[0].SpawnTileId = 9;

        rs.units[1].PID = 1;
        player.unit2.slotID = rs.units[1].slotID = 2;
        player.unit2.WeaponID = rs.units[1].WeaponID = static_cast<std::uint8_t>(s[0].unitId2);
        player.unit2.hp = rs.units[1].hp = 100;
        rs.units[1].stamina = 100;
        player.unit2.tileID = rs.units[1].SpawnTileId = 23;

        // MK 추가
        allPlayerUnits.emplace(player.PID, player);

        // slot1 (P2)
        player.PID = rs.units[2].PID = 2;
        player.unit1.slotID = rs.units[2].slotID = 1;
        player.unit1.WeaponID = rs.units[2].WeaponID = static_cast<std::uint8_t>(s[1].unitId1);
        player.unit1.hp = rs.units[2].hp = 100;
        player.stamina = rs.units[2].stamina = 100;
        player.unit1.tileID = rs.units[2].SpawnTileId = 13;

        rs.units[3].PID = 2;
        unit2.slotID = rs.units[3].slotID = 2;
        unit2.WeaponID = rs.units[3].WeaponID = static_cast<std::uint8_t>(s[1].unitId2);
        unit2.hp = rs.units[3].hp = 100;
        rs.units[3].stamina = 100;
        unit2.tileID = rs.units[3].SpawnTileId = 27;

        // MK 추가
        allPlayerUnits.emplace(player.PID, player);

        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_RoundStart,
            [&](yuno::net::ByteWriter& w)
            {
                rs.Serialize(w);
            });

        m_network.Broadcast(std::move(bytes));
    }
}
