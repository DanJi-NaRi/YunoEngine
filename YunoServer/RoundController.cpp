#include "RoundController.h"

#include "MatchManager.h"
#include "ServerCardDealer.h"
#include "YunoServerNetwork.h"
#include "PlayerCardController.h"
#include "BattleState.h"

#include "PacketBuilder.h"
#include "S2C_CountDown.h"
#include "S2C_RoundStart.h"
#include "S2C_CardPackets.h"
#include "S2C_EndGame.h"

#include <iostream>

namespace yuno::server
{
    RoundController::RoundController(
        MatchManager& match,
        ServerCardDealer& dealer,
        YunoServerNetwork& network,
        PlayerCardController& cardController)
        : m_match(match)
        , m_cardDealer(dealer)
        , m_network(network)
        , m_cardController(cardController)
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

        m_match.InitBattleState();
        for (int i = 0; i < 2; ++i)
        {
            m_cardController.InitPlayerCards(g_battleState.players[i]);
        }

        SendInitialCards();

        SendCountDown();

        SendRoundStart();       

        StartTurn();
    }

    // ------------------------------------------------------

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
        std::vector<yuno::net::packets::CardSpawnInfo> allCards;

        for (int p = 0; p < 2; ++p)
        {
            auto& player = g_battleState.players[p];

            allCards.insert(
                allCards.end(),
                player.handCards.begin(),
                player.handCards.end());
        }

        yuno::net::packets::S2C_StartCardList pkt;
        pkt.cards = allCards;

        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_StartCardList,
            [&](yuno::net::ByteWriter& w)
            {
                pkt.Serialize(w);
            });

        m_network.Broadcast(std::move(bytes));
    }

    // ------------------------------------------------------

    void RoundController::SendRoundStart()
    {
        const auto& s = m_match.Slots();

        yuno::net::packets::S2C_RoundStart rs{};
     
        for (int p = 0; p < 2; ++p)
        {
            auto& player = g_battleState.players[p];
            const auto& slot = s[p];

            player.PID = static_cast<uint8_t>(p + 1);

            for (int u = 0; u < 2; ++u)
            {
                const int unitIndex = p * 2 + u; 
                auto& rsUnit = rs.units[unitIndex];
                auto& unit = (u == 0) ? player.unit1 : player.unit2;

                rsUnit.PID = player.PID;
                rsUnit.slotID = static_cast<uint8_t>(u + 1);
                rsUnit.WeaponID = unit.WeaponID;
                rsUnit.hp = unit.hp;
                rsUnit.stamina = unit.stamina;
                rsUnit.SpawnTileId = unit.tileID;
            }
        }

        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_RoundStart,
            [&](yuno::net::ByteWriter& w)
            {
                rs.Serialize(w);
            });

        m_network.Broadcast(std::move(bytes));
    }

    void RoundController::SendDrawCandidates()
    {
        for (int p = 0; p < 2; ++p)
        {
            auto& player = g_battleState.players[p];

            m_cardController.DrawCards(player);

            auto session = m_network.FindSession(player.sessionId);

            yuno::net::packets::S2C_DrawCandidates pkt;
            pkt.cards = player.drawCandidates;

            auto bytes = yuno::net::PacketBuilder::Build(
                yuno::net::PacketType::S2C_DrawCandidates,
                [&](yuno::net::ByteWriter& w)
                {
                    pkt.Serialize(w);
                });
            if(session==nullptr) return;
            session->Send(std::move(bytes));

            std::cout << "[Server] SendDrawCandidates to PID="
                << int(player.PID)
                << " count=" << pkt.cards.size()
                << "\n";
        }
    }

    void RoundController::EndRound()
    {
        if (g_battleState.matchEnded || g_battleState.currentRound > 3)
            EndGame();
        else
            TryStartRound();
    }

    void RoundController::StartTurn()
    {
        ++m_turnNumber;
        std::cout << "[Round] StartTurn #" << m_turnNumber << "\n";

        yuno::net::packets::S2C_StartTurn pkt{};
        pkt.turnNumber = m_turnNumber;

        // 두 플레이어가 선택한 카드 결과
        pkt.addedCards[0] = g_battleState.players[0].handCards.back();
        pkt.addedCards[1] = g_battleState.players[1].handCards.back();

        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_StartTurn,
            [&](yuno::net::ByteWriter& w)
            {
                pkt.Serialize(w);
            });

        m_network.Broadcast(std::move(bytes));
    }

    void RoundController::EndTurn()
    {
        std::cout << "[Round] EndTurn\n";

        m_cardSelected[0] = false;
        m_cardSelected[1] = false;

        SendDrawCandidates();   


        if (g_battleState.roundEnded)
            EndRound(); 
    }

    void RoundController::EndGame()
    {
        std::cout << "[Gmae] EndGame " << "\n";

        yuno::net::packets::S2C_EndGame pkt{};

        // P1
        pkt.results[0].PID = 1;
        pkt.results[0].winCount = g_battleState.roundWins[0];

        // P2
        pkt.results[1].PID = 2;
        pkt.results[1].winCount = g_battleState.roundWins[1];

        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_EndGame,
            [&](yuno::net::ByteWriter& w)
            {
                pkt.Serialize(w);
            });

        m_network.Broadcast(std::move(bytes));
    }
    
    void RoundController::OnPlayerSelectedCard(int playerIdx)
    {
        if (m_cardSelected[playerIdx])
            return; // 중복 선택 방지

        m_cardSelected[playerIdx] = true;

        std::cout << "[Round] Player " << playerIdx
            << " selected bonus card\n";

        // 둘 다 선택 완료 확인
        if (m_cardSelected[0] && m_cardSelected[1])
        {
            StartTurn();
        }
    }
}
