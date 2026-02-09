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
#include "S2C_EndGame_Disconnect.h"
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

    void RoundController::Update()
    {
        if (!m_waitingRoundStartReady)
            return;

        if (std::chrono::steady_clock::now() < m_roundStartReadyDeadline)
            return;

        auto remainingMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                m_roundStartReadyDeadline - std::chrono::steady_clock::now());

        std::cout << "remaining: " << remainingMs.count() << " ms" << std::endl;
        std::cout << "[Round] RoundStartReady timeout. Force starting next round\n";

        m_waitingRoundStartReady = false;
        TryStartRound();
    }

    void RoundController::OnRoundStartReadyOK(int playerIdx)
    {
        if (playerIdx < 0 || playerIdx > 1)
            return;

        if (!m_waitingRoundStartReady)
            return;

        if (m_roundStartReady[playerIdx])
            return;

        m_roundStartReady[playerIdx] = true;

        std::cout << "[Round] RoundStartReadyOK from player " << playerIdx << "\n";

        if (!(m_roundStartReady[0] && m_roundStartReady[1]))
            return;

        m_waitingRoundStartReady = false;
        TryStartRound();
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
            m_matchLocked = true;
            m_match.ClearReadyAndUnits();

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

        if (g_battleState.matchEnded || g_battleState.currentRound > 3)
        {
            EndGame();
            return;
        }

        m_waitingRoundStartReady = true;
        m_roundStartReady[0] = false;
        m_roundStartReady[1] = false;
        m_roundStartReadyDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(50);

        //else 
        //{
        //    TryStartRound(); // 이거 자동으로 시작되게 하는거 바꿔야 될듯
        //} 
        // 이거 바꾼 이유 = 클라이언트에서 라운드 종료시 기존 유닛을 파괴하고 새로운 유닛을 생성해서
        // 보유 유닛 벡터에 집어넣을 텐데 서버가 보내자마자 해당 행동을 시작해서 자동시작말고 클라에서 유닛 파괴하면
        // 서버한테 C2S_RoundStartReadyOK 패킷을 보내서 알려주기로 바꿈
        // 게임 크래쉬와 연관된 부분이라서 안전하게 패킷으로 통신해서 처리해야 된다고 판단

    }


    void RoundController::StartTurn()
    {



        yuno::net::packets::S2C_StartTurn pkt{};
        pkt.turnNumber = ++g_battleState.turnNumber;        // 1턴부터 시작


        std::cout << "[Round] StartTurn #" << g_battleState.turnNumber << "\n";

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
        std::cout << "[Round] EndTurn , round Ended data : " << g_battleState.roundEnded << std::endl;

        if (g_battleState.roundEnded)
        {
            std::cout << "-----Enter EndRound()-----" << std::endl;
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

        std::cout << "[Round] Match ended. Reset loop state for next match\n";
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
        // 여기서 엔딩씬으로 넘어갈거면 엔딩씬 넘기는 패킷에 데이터 담아서 보내고나서 
        // 데이터 리셋 ㄱㄱ

        ResetMatchState();

    }

    void RoundController::EndGameByDisconnect(uint8_t winnerPID)
    {
        std::cout << "[Round] Match ended by disconnect. winner="
            << int(winnerPID) << "\n";

        yuno::net::packets::S2C_EndGame_Disconnect pkt{};
        pkt.winnerPID = winnerPID;

        auto bytes = yuno::net::PacketBuilder::Build(
            yuno::net::PacketType::S2C_EndGame_Disconnect,
            [&](yuno::net::ByteWriter& w)
            {
                pkt.Serialize(w);
            });

        m_network.Broadcast(std::move(bytes));

        ResetMatchState();
    }

    void RoundController::ResetMatchState()
    {
        m_matchLocked = false;
        m_roundStarted = false;
        m_cardsInitialized = false;
        m_waitingRoundStartReady = false;
        m_roundStartReady[0] = false;
        m_roundStartReady[1] = false;
        m_cardSelected[0] = false;
        m_cardSelected[1] = false;

        g_battleState.turnNumber = 0;
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
            player.sessionId = 0;
            player.PID = 0;
            player.unit1 = {};
            player.unit2 = {};
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
