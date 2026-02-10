#include "pch.h"

#include "YunoClientNetwork.h"

// 이거로 게임 관리할거임
#include "GameManager.h"
#include "Card.h"

#include "PacketBuilder.h"

// 패킷들
#include "S2C_Pong.h"
#include "S2C_EnterOK.h"
#include "S2C_Error.h"
#include "S2C_ReadyState.h"
#include "S2C_CountDown.h"
#include "S2C_RoundStart.h"
#include "S2C_BattlePackets.h"

#include "C2S_SubmitWeapon.h"

#include "S2C_CardPackets.h"
#include "S2C_EndGame.h"
#include "S2C_Emote.h"
#include "S2C_EndGame_Disconnect.h"
namespace yuno::game
{
    YunoClientNetwork::YunoClientNetwork()
        : m_workGuard(boost::asio::make_work_guard(m_io))
        , m_client(m_io)
    {

        m_serverPeer.sId = 0;

        m_client.SetOnPacket(
            [this](std::vector<std::uint8_t>&& packet)
            {
                // 이 콜백은 io_context 스레드에서 호출됨
                PushIncoming(std::move(packet));
            });

        m_client.SetOnDisconnected(
            [this](const boost::system::error_code& /*ec*/)
            {
                // 필요하면 "끊김 이벤트"를 별도 큐로도 올릴 수 있음
            });
    }

    YunoClientNetwork::~YunoClientNetwork()
    {
        Stop();
    }

    void YunoClientNetwork::Start(const std::string& host, std::uint16_t port)
    {
        if (m_running.exchange(true))
            return;

        // io_context 실행 스레드 시작
        m_netThread = std::thread(
            [this]()
            {
                m_io.run();
            });

        // 연결 시도는 네트워크 스레드 컨텍스트에서 실행되도록 post
        boost::asio::post(
            m_io,
            [this, host, port]()
            {
                m_client.Connect(host, port);
            });
    }

    void YunoClientNetwork::Stop()
    {
        if (!m_running.exchange(false))
            return;

        // Disconnect도 io 스레드에서 처리
        boost::asio::post(
            m_io,
            [this]()
            {
                m_client.Disconnect();
            });

        m_workGuard.reset();
        m_io.stop();

        if (m_netThread.joinable())
            m_netThread.join();

        // 큐 정리
        {
            std::lock_guard<std::mutex> lock(m_inMtx);
            m_inQ.clear();
        }
    }

    bool YunoClientNetwork::IsConnected() const
    {
        return m_client.IsConnected();
    }

    void YunoClientNetwork::SendPacket(std::vector<std::uint8_t> packetBytes)
    {
        boost::asio::post(
            m_io,
            [this, bytes = std::move(packetBytes)]() mutable
            {
                m_client.Send(std::move(bytes));
            });
    }

    void YunoClientNetwork::PumpIncoming(float dt)
    {
        // 메인 스레드에서만 호출
        std::vector<std::uint8_t> pkt;
        while (PopIncoming(pkt))
        {
            m_dispatcher.Dispatch(m_serverPeer, pkt);
        }
    }

    void YunoClientNetwork::PushIncoming(std::vector<std::uint8_t>&& packetBytes)
    {
        std::lock_guard<std::mutex> lock(m_inMtx);
        m_inQ.emplace_back(std::move(packetBytes));
    }

    bool YunoClientNetwork::PopIncoming(std::vector<std::uint8_t>& out)
    {
        std::lock_guard<std::mutex> lock(m_inMtx);
        if (m_inQ.empty())
            return false;

        out = std::move(m_inQ.front());
        m_inQ.pop_front();
        return true;
    }


    // ------------------------------- 핸들 함수 등록 -------------------------------
    void YunoClientNetwork::RegisterMatchPacketHandler() 
    {

        using namespace yuno::net;

        Dispatcher().RegisterRaw(
            PacketType::S2C_Pong,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                if (bodyLen < 4)
                    return;

                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_Pong::Deserialize(r);
                std::cout << "InComing Data : " << pkt.nonce << std::endl;

            });

        // EnterOK Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_EnterOK,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_EnterOK::Deserialize(r);

                std::cout << "Slot Idx : " << static_cast<int>(pkt.slotIndex) << ", Player Count : " << static_cast<int>(pkt.playerCount) << std::endl;

                GameManager& gm = GameManager::Get();
                gm.SetMatchPlayerCount(pkt.playerCount);

                if (gm.GetSlotiIdx() == 0)
                {
                    gm.SetSlotIdx(pkt.slotIndex);
                }

                if (gm.GetSceneState() == CurrentSceneState::RequstEnter)
                {
                    gm.SetSceneState(CurrentSceneState::GameStart);
                }
            });// EnterOK Packet End

        // ReadyState Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_ReadyState,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_ReadyState::Deserialize(r);

                const bool p1Ready = (pkt.p1_isReady != 0);
                const bool p2Ready = (pkt.p2_isReady != 0);

                GameManager& gm = GameManager::Get();
                gm.SetReadyStates(p1Ready, p2Ready);

                if (!(p1Ready && p2Ready))
                {
                    return;
                }
                    


                const std::uint32_t weapon1 = static_cast<std::uint32_t>(gm.GetMyPiece(0));
                const std::uint32_t weapon2 = static_cast<std::uint32_t>(gm.GetMyPiece(1));
                if (weapon1 == 0 || weapon2 == 0) 
                {
                    return;
                }


                yuno::net::packets::C2S_SubmitWeapon req{};
                req.WeaponId1 = weapon1;
                req.WeaponId2 = weapon2;

                auto bytes = PacketBuilder::Build(
                    PacketType::C2S_SubmitWeapon,
                    [&](ByteWriter& w)
                    {
                        req.Serialize(w);
                    });

                SendPacket(std::move(bytes));
            });// ReadyState Packet End

        // CountDown Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_CountDown,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_CountDown::Deserialize(r);

                const int countTime = static_cast<int>(pkt.countTime);
                if (countTime <= 0)
                    return;

                GameManager& gm = GameManager::Get();

                std::cout << "game state : " << static_cast<int>(gm.GetSceneState()) << std::endl;
                // 현재 씬이 StandBy 상태 즉 WeaponSelectScene일때만 패킷 동작
                if (gm.GetSceneState() == CurrentSceneState::StandBy) {
                    gm.StartCountDown(
                        countTime,
                        pkt.slot1_UnitId1, pkt.slot1_UnitId2,
                        pkt.slot2_UnitId1, pkt.slot2_UnitId2
                    );
                }

            });// CountDown Packet End
        
        // RoundStart Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_RoundStart,
            [this](const NetPeer& peer, const PacketHeader& header, const std::uint8_t* body, std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::S2C_RoundStart::Deserialize(r);

                GameManager& gm = GameManager::Get();

                gm.SetBattleOngoing(true);

                gm.ResetWeaponData();

                for (const auto& u : pkt.units)
                {
                    gm.SetWeaponData(u.PID, u.slotID, u.WeaponID, u.hp, u.stamina, u.SpawnTileId);
                }
                
                gm.SetUpPanels(); // 패널 초기화

                auto wVector = gm.GetWeaponData();

                for (const auto& data : wVector) 
                {
                    std::cout << "Weapon data : " << data.pId << " ," << data.slotId << " ," << data.weaponId << " ," << data.hp << " ," << data.stamina << " ," << data.currentTile << std::endl;
                }

            });// RoundStart Packet End

        // Error Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_Error,
            [this](const NetPeer& /*peer*/,
                const PacketHeader& /*header*/,
                const std::uint8_t* body, std::uint32_t bodyLen)
            {
                if (body == nullptr || bodyLen < 4)
                {
                    std::cout << "[Client] S2C_Error invalid bodyLen=" << bodyLen << "\n";
                    return;
                }
                 
                ByteReader r(body, bodyLen);
                const auto err = yuno::net::packets::S2C_Error::Deserialize(r);

                const auto code = err.code;         // 어떤 에러가 났는지
                const auto reason = err.reason;     // 왜 발생했는지
                const auto ctx = err.contextType;   // 어떤 패킷에서 발생했는지


                // MatchEnter 거부 처리
                if (code == yuno::net::packets::ErrorCode::EnterDenied &&
                    ctx == PacketType::C2S_MatchEnter)
                {

                    if (reason == yuno::net::packets::EnterDeniedReason::RoomFull)
                    {
                        std::cout << "[Client] Enter denied: RoomFull\n";

                        return;
                    }
                    else if (reason == yuno::net::packets::EnterDeniedReason::AlreadyInMatch)
                    {
                        std::cout << "[Client] Enter denied: AlreadyInMatch\n";

                        return;
                    }

                    return;
                }

            } );// Error Packet End
    
        // TestCardList Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_StartCardList,
            [this](const NetPeer& peer,
                const PacketHeader& header,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt =
                    yuno::net::packets::S2C_StartCardList::Deserialize(r);

                GameManager::Get().InitHands(pkt.cards);

                std::cout << "[Client] TestCardList stored. count="
                    << pkt.cards.size() << "\n";

                for (size_t i = 0; i < pkt.cards.size(); ++i)
                {
                    const auto& card = pkt.cards[i];
                    std::cout << "  [" << i << "] runtimeID="
                        << card.runtimeID
                        << ", dataID="
                        << card.dataID
                        << "\n";
                }
            }
        ); // TestCardList Packet End

        // BattleResult Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_BattleResult,
            [this](const NetPeer& peer,
                const PacketHeader& header,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                if (body == nullptr || bodyLen == 0)
                    return;

                ByteReader r(body, bodyLen);
                const auto pkt =
                    yuno::net::packets::S2C_BattleResult::Deserialize(r);

                GameManager& gm = GameManager::Get();

                // 어떤 카드 결과인지 (지금은 로그용)
                std::cout << "[Client] BattleResult runtimeCardId="
                    << pkt.runtimeCardId
                    << " ownerSlot=" << static_cast<int>(pkt.ownerSlot)
                    << "\n";

                for (auto& data : pkt.order) 
                {
                    std::cout << "tileId(1,2,3,4) : " << static_cast<int>(data[0].targetTileID) << ", "
                        << static_cast<int>(data[1].targetTileID) << ", "
                        << static_cast<int>(data[2].targetTileID) << ", "
                        << static_cast<int>(data[3].targetTileID) << std::endl;
                }

                std::vector<std::array<UnitState, 4>> order;
                for (int i = 0; i < pkt.order.size(); i++)
                {
                    const auto& u = pkt.order[i];
                    std::array<UnitState, 4> us;
                    for (int j = 0; j < us.size(); j++)
                    {
                        us[j] = { u[j].ownerSlot, u[j].unitLocalIndex, u[j].hp,
                                  u[j].stamina, u[j].targetTileID, u[j].isEvent };
                    }
                    order.push_back(us);
                }

                BattleResult br{ pkt.runtimeCardId, pkt.ownerSlot, pkt.unitLocalIndex, pkt.dir, pkt.actionTime, order };
                gm.PushBattlePacket(br);
                gm.UpdatePanels(br);
                gm.SetSceneState(CurrentSceneState::AutoBattle);
                // MK 추가
                // 게임 매니저 큐에 push.


                // 디버깅용
                std::cout
                    << "\n------------------------------\n"
                    << "    [BattleResult Packet]"
                    << "\n runTimeCardID = " << static_cast<int>(br.runTimeCardID)
                    << "\n PID = " << static_cast<int>(br.pId)
                    << "  ,  Unit = " << static_cast<int>(br.slotId)
                    << "\n Dir = " << static_cast<int>(br.dir)
                    << "  ,  ActionTime = " << static_cast<int>(br.actionTime);
                for (const auto& d : order)
                {
                    for (int i = 0; i < d.size(); i++)
                    {
                        const int slot = static_cast<int>(d[i].pId);
                        const int unit = static_cast<int>(d[i].slotId);

                        std::cout
                            << "\n\n      <UnitState" << i << "> \n"
                            << " Player = " << slot
                            << "  ,  unit =" << unit
                            << "\n hp =" << static_cast<int>(d[i].hp)
                            << "  ,  stamina =" << static_cast<int>(d[i].stamina)
                            << "\n move =" << static_cast<int>(d[i].targetTileID)
                            << "  ,  isEvent =(" << static_cast<bool>(d[i].isEvent) << ")\n";
                    }
                    std::cout << "------------------------------\n\n";
                }
            }
        );// BattleResult Packet End

        // DrawCandidates Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_DrawCandidates,
            [this](const NetPeer& peer,
                const PacketHeader& header,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                if (body == nullptr || bodyLen == 0)
                    return;

                ByteReader r(body, bodyLen);
                const auto pkt =
                    yuno::net::packets::S2C_DrawCandidates::Deserialize(r);

                GameManager::Get().SetDrawCandidates(pkt.cards);

                for (size_t i = 0; i < pkt.cards.size(); ++i)
                {
                    const auto& card = pkt.cards[i];
                    std::cout
                        << "  [" << i << "]"
                        << " PID=" << static_cast<int>(card.PID)
                        << " UnitSlot=" << static_cast<int>(card.slotID)
                        << " runtimeID=" << card.runtimeID
                        << " dataID=" << card.dataID
                        << "\n";
                }

                // 나중에 여기서 GameManager로 넘기면 됨
                // GameManager::Get().SetDrawCandidates(pkt.cards);
            }
        ); // DrawCandidates Packet End

        // S2C_StartTurn Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_StartTurn,
            [this](const NetPeer& peer,
                const PacketHeader& header,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                if (body == nullptr || bodyLen == 0)
                    return;

                ByteReader r(body, bodyLen);
                const auto pkt =
                    yuno::net::packets::S2C_StartTurn::Deserialize(r);

                std::cout << "\n[Client] === S2C_StartTurn ===\n";
                std::cout << "[Client] TurnNumber = "
                    << static_cast<int>(pkt.turnNumber) << "\n";

                std::vector<yuno::net::packets::CardSpawnInfo> added;
                added.push_back(pkt.addedCards[0]);
                added.push_back(pkt.addedCards[1]);

                GameManager::Get().AddCards(added);
                GameManager::Get().ClearDrawCandidates();
                GameManager::Get().ClearCardQueue();

                for (int i = 0; i < 2; ++i)
                {
                    const auto& c = pkt.addedCards[i];

                    std::cout
                        << "[Client] AddedCard[" << i << "] "
                        << " PID=" << static_cast<int>(c.PID)
                        << " slotID=" << static_cast<int>(c.slotID)
                        << " runtimeID=" << c.runtimeID
                        << " dataID=" << c.dataID
                        << "\n";
                }

                std::cout << "[Client] ======================\n";
            }
        );// S2C_StartTurn Packet End

        // S2C_EndGame Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_EndGame,
            [](const NetPeer& /*peer*/,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                if (body == nullptr || bodyLen == 0)
                    return;

                yuno::net::ByteReader r(body, bodyLen);
                const auto pkt =
                    yuno::net::packets::S2C_EndGame::Deserialize(r);

                std::cout << "=============================\n";
                std::cout << "[Client] S2C_EndGame received\n";

                GameManager::Get().SetBattleOngoing(false);

                for (int i = 0; i < 2; ++i)
                {
                    std::cout
                        << " Player PID=" << int(pkt.results[i].PID)
                        << " winCount=" << int(pkt.results[i].winCount)
                        << "\n";
                }

                // 결과 판정
                const uint8_t p1Wins = pkt.results[0].winCount;
                const uint8_t p2Wins = pkt.results[1].winCount;

                int winnerPID = 0;

                if (p1Wins > p2Wins)
                {
                    winnerPID = 1;
                    std::cout << "[Result] Player 1 WIN\n";
                }
                else if (p1Wins < p2Wins)
                {
                    winnerPID = 2;
                    std::cout << "[Result] Player 2 WIN\n";
                }   
                else
                {
                    winnerPID = -1;
                    std::cout << "[Result] DRAW\n";
                }
                std::cout << "=============================\n";

                GameManager::Get().SetWinnerPID(winnerPID);
                GameManager::Get().SetSceneState(CurrentSceneState::ResultScene);
            }
        );// S2C_EndGame Packet End

        // S2C_Emote Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_Emote,
            [](const NetPeer&,
                const PacketHeader&,
                const uint8_t* body,
                uint32_t bodyLen)
            {
                if (body == nullptr || bodyLen == 0)
                    return;

                yuno::net::ByteReader r(body, bodyLen);
                const auto pkt =
                    yuno::net::packets::S2C_Emote::Deserialize(r);

                std::cout
                    << "[Client] Emote Received "
                    << "from PID=" << int(pkt.PID)
                    << " emoteId=" << int(pkt.emoteId)
                    << "\n";

                GameManager::Get().PushEmote(pkt.PID, pkt.emoteId);
            }
        );// S2C_Emote Packet End

        Dispatcher().RegisterRaw(//S2C_ObstacleResult Packet Start
            PacketType::S2C_ObstacleResult,
            [](const NetPeer&,
                const PacketHeader&,
                const uint8_t* body,
                uint32_t bodyLen)
            {
                if (!body || bodyLen == 0)
                    return;

                yuno::net::ByteReader r(body, bodyLen);
                auto pkt =
                    yuno::net::packets::S2C_ObstacleResult::Deserialize(r);

                //여기서 게임매니저에 만든 함수 가져와서 클라에 나올거 구현하기

                std::cout
                    << "[Client] ObstacleResult received "
                    << " count=" << pkt.obstacles.size()
                    << std::endl;

                auto& gm = GameManager::Get();

                for (const auto& o : pkt.obstacles)
                {
                    std::cout
                        << "  obstacleID=" << int(o.obstacleID)
                        << " tiles=" << o.tileIDs.size();

                    if (!o.tileIDs.empty())
                    {
                        std::cout << " [";
                        for (size_t i = 0; i < o.tileIDs.size(); ++i)
                        {
                            std::cout << int(o.tileIDs[i]);
                            if (i + 1 != o.tileIDs.size())
                                std::cout << ", ";
                        }
                        std::cout << "]";
                    }
                    std::cout << "\n";

                    ObstacleResult out{};

                    out.obstacleID = static_cast<ObstacleType>(o.obstacleID);

                    out.tileIDs = o.tileIDs;

                    for (int i = 0; i < static_cast<int>(out.unitState.size()); ++i)
                    {
                        out.unitState[i] = {
                            o.unitState[i].ownerSlot,
                            o.unitState[i].unitLocalIndex,
                            o.unitState[i].hp,
                            o.unitState[i].stamina,
                            o.unitState[i].targetTileID,
                            o.unitState[i].isEvent
                        };

                        std::cout
                            << "    triggerUnit[" << i << "]"
                            << " pId=" << int(out.unitState[i].pId)
                            << " slot=" << int(out.unitState[i].slotId)
                            << " hp=" << int(out.unitState[i].hp)
                            << " stamina=" << int(out.unitState[i].stamina)
                            << " tile=" << int(out.unitState[i].targetTileID)
                            << " isEvent=" << int(out.unitState[i].isEvent)
                            << "\n";
                    }
                    gm.UpdatePanels(out);
                    gm.PushObstaclePacket(out);
                }
            }
        );//S2C_ObstacleResult Packet End

        //S2C_EndGame_Disconnect Packet Start
        Dispatcher().RegisterRaw(
            PacketType::S2C_EndGame_Disconnect,
            [this](const NetPeer&, const PacketHeader&, const uint8_t* body, uint32_t len)
            {
                ByteReader r(body, len);
                auto pkt = yuno::net::packets::S2C_EndGame_Disconnect::Deserialize(r);

                std::cout << "winnerPID : " <<  static_cast<int>(pkt.winnerPID) << std::endl;

                GameManager::Get().SetBattleOngoing(false);
                GameManager::Get().SetWinnerPID(pkt.winnerPID);
                GameManager::Get().SetSceneState(CurrentSceneState::ResultScene);
            }
        ); //S2C_EndGame_Disconnect Packet End
    }
}
