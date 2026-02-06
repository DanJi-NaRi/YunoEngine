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

        m_match.InitBattleState();  // 라운드초기화

        if (!m_cardsInitialized)
        {
            for (int i = 0; i < 2; ++i)
            {
                m_cardController.InitPlayerCards(g_battleState.players[i]); // 1회 초기화
            }

            SendInitialCards(); // 1회 초기화
            SendCountDown(); // 1회 초기화
            m_cardsInitialized = true;
        }

        SendRoundStart(); // 라운드초기화

        StartTurn(); // 라운드초기화
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
        m_roundStarted = false;

        if (g_battleState.matchEnded)   // 매치(게임)이 끝났으면 엔드게임
            EndGame();
        else                            // 아직 안끝났으면 라운드 스타트
        {
            TryStartRound();
        }

    }    

    void RoundController::StartTurn()
    {
        ++m_turnNumber;
        std::cout << "[Round] StartTurn #" << m_turnNumber << "\n";

        yuno::net::packets::S2C_StartTurn pkt{};
        pkt.turnNumber = g_battleState.turnNumber;

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
        std::cout << "[Round] EndTurn , data : " << g_battleState.roundEnded << std::endl;

        if (g_battleState.roundEnded)
        {
            EndRound();
            return;
        }
        m_cardSelected[0] = false;
        m_cardSelected[1] = false;

        SendDrawCandidates();
        //종료 플래그 이미 받은 상태
        //턴스타트 라운드엔드
    }

    void RoundController::EndGame()
    {

        // 여기서 엔딩씬으로 넘어갈거면 엔딩씬 넘기는 패킷에 데이터 담아서 보내고나서 
        // 데이터 리셋 ㄱㄱ
        std::cout << "[Round] Match ended. Reset loop state for next match\n";

        m_roundStarted = false;
        m_cardsInitialized = false;
        m_turnNumber = 0;
        m_cardSelected[0] = false;
        m_cardSelected[1] = false;

        g_battleState.roundWins[0] = 0;
        g_battleState.roundWins[1] = 0;
        g_battleState.currentRound = 1;

        g_battleState.roundEnded = false;
        g_battleState.roundWinnerPID = 0;
        g_battleState.roundLoserPID = 0;

        g_battleState.matchEnded = false;
        g_battleState.matchWinnerPID = 0;

        for (int i = 0; i < 2; ++i)
        {
            auto& player = g_battleState.players[i];
            player.remainingDeck.clear();
            player.handCards.clear();
            player.drawCandidates.clear();
        }

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
