#include "YunoServerNetwork.h"

#include "PacketBuilder.h"

#include "C2S_Ping.h"
#include "C2S_MatchEnter.h"
#include "C2S_MatchLeave.h"
#include "C2S_SubmitWeapon.h"
#include "C2S_ReadySet.h"
#include "C2S_BattlePackets.h"

#include "S2C_Pong.h"
#include "S2C_EnterOK.h"
#include "S2C_Error.h"
#include "S2C_CountDown.h"
#include "S2C_ReadyState.h"
#include "S2C_RoundStart.h"
#include "S2C_CardPackets.h"


#include <iostream>
namespace yuno::server
{
    void YunoServerNetwork::Broadcast(std::vector<std::uint8_t>&& bytes)
    {
        m_server.Broadcast(std::move(bytes));
    }

    std::shared_ptr<yuno::net::TcpSession>
        YunoServerNetwork::FindSession(std::uint64_t sessionId)
    {
        return m_server.FindSession(sessionId);
    }

    YunoServerNetwork::YunoServerNetwork()
        : m_io()
        , m_server(m_io)
        , m_match()
        , m_cardDB()
        , m_cardRangeDB()
        , m_cardRuntime()
        , m_cardDealer(m_cardDB, m_cardRuntime)
        , m_turnManager(m_match, *this, m_cardRuntime, m_cardDB, m_cardRangeDB, m_roundController)
        , m_roundController(m_match, m_cardDealer, *this, m_cardController)
        , m_cardController(m_cardDealer, m_cardDB, m_cardRuntime)

    {


        m_server.SetOnPacket(
            [this](std::shared_ptr<yuno::net::TcpSession> session, std::vector<std::uint8_t>&& packetBytes)
            {
                std::cout << "-----------[Server] OnPacket: sid=" << session->GetSessionId()
                    << ", bytes=" << packetBytes.size() << "\n";

                yuno::net::NetPeer peer{};
                peer.sId = session->GetSessionId();              

                m_dispatcher.Dispatch(peer, packetBytes);
                std::cout << "[Server] Dispatch called\n";
            });

        m_server.SetOnDisconnected(
            [this](std::shared_ptr<yuno::net::TcpSession> session, const boost::system::error_code& ec)
            {
                const auto sid = static_cast<uint64_t>(session->GetSessionId());

                m_match.DetachBySessionId(sid);

                std::cout << "[Server] Disconnected sid=" << sid
                    << " ec=" << ec.message()
                    << " -> slot freed\n";
            });
    }

    YunoServerNetwork::~YunoServerNetwork()
    {
        Stop();
    }

    bool YunoServerNetwork::Start(std::uint16_t port)
    {
        std::cout << "[Server] Start called. port=" << port << "\n";

        if (!m_cardDB.LoadFromCSV("../Assets/CardData/CardData.csv"))       // 카드 데이터 로드
        {
            std::cerr << "[Server] Card CSV load failed\n";
            return false;
        }
        if (!m_cardRangeDB.LoadFromCSV("../Assets/CardData/CardRange.csv")) // 카드 범위 데이터 로드
        {
            std::cerr << "[Server] cardRange CSV load failed\n";
            return false;
        }

        std::cout << "[Server] Init OK: "
            << "Cards=" << m_cardDB.GetAllCards().size()
            << ", Ranges=" << m_cardRangeDB.GetRangeCount()
            << "\n";

        const bool ok = m_server.Start(port);
        std::cout << "[Server] TcpServer::Start result=" << (ok ? "true" : "false") << "\n";

        RegisterMatchPacketHandler();

        return ok;
    }


    void YunoServerNetwork::Stop()
    {
        m_server.Stop();
    }


    void YunoServerNetwork::Tick()
    {
        const std::size_t n = m_io.poll_one();
    }


    // ------------------------------- 핸들 함수 등록 -------------------------------
    void YunoServerNetwork::RegisterMatchPacketHandler()
    {
        using namespace yuno::net;

        // Ping Packet Start
        m_dispatcher.RegisterRaw(
            PacketType::C2S_Ping,
            [this](const NetPeer& peer,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                std::cout << "[Server] C2S_Ping handler called. sid=" << peer.sId << "\n";

                if (bodyLen < 4) return;

                ByteReader r(body, bodyLen);
                const auto ping = yuno::net::packets::C2S_Ping::Deserialize(r);

                // 데이터 핸들링
                yuno::net::packets::S2C_Pong pong{};
                pong.nonce = ping.nonce * 3;

                // 응답 패킷 생성
                auto bytes = PacketBuilder::Build(
                    PacketType::S2C_Pong,
                    [&](ByteWriter& w)
                    {
                        pong.Serialize(w);
                    });

                // 응답 패킷 전송
                // 브로드캐스트 방식
                // m_server.Broadcast(std::move(bytes));

                // 유니캐스트 방식
                auto session = m_server.FindSession(peer.sId);
                if (!session) return;

                session->Send(std::move(bytes));
            }
        ); // Ping Packet End

        // Enter Packet Start
        m_dispatcher.RegisterRaw(
            PacketType::C2S_MatchEnter,
            [this](const NetPeer& peer,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                yuno::net::ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::C2S_MatchEnter::Deserialize(r);

                auto session = m_server.FindSession(peer.sId);
                if (!session) return;

                const int slotIdx = m_match.AllocateOrAttachUser(pkt.userId, peer.sId);

                if (slotIdx == -1 ) // 1P, 2P 둘 다 있으면 에러 패킷 전송
                {
                    yuno::net::packets::S2C_Error error{};

                    error.code = yuno::net::packets::ErrorCode::EnterDenied;
                    error.reason = yuno::net::packets::EnterDeniedReason::RoomFull;
                    error.contextType = PacketType::C2S_MatchEnter;

                    auto bytes = yuno::net::PacketBuilder::Build(
                        yuno::net::PacketType::S2C_Error,
                        [&](yuno::net::ByteWriter& w)
                        {
                            error.Serialize(w);
                        });

                    auto session = m_server.FindSession(peer.sId);
                    if (!session) return;

                    session->Send(std::move(bytes));
                }
                else                                            // 빈 자리 존재
                {
                    yuno::net::packets::S2C_EnterOK ok{};
                    ok.slotIndex = static_cast<std::uint8_t>(slotIdx+1);
                    ok.playerCount = m_match.GetOccupiedCount();

                    auto bytes = yuno::net::PacketBuilder::Build(
                        yuno::net::PacketType::S2C_EnterOK,
                        [&](yuno::net::ByteWriter& w)
                        {
                            ok.Serialize(w);
                        });

                    session->Send(std::move(bytes));
                    
                }
            }
        ); // Enter Packet End

        // Leave Packet Start
        m_dispatcher.RegisterRaw(
            PacketType::C2S_MatchLeave,
            [this](const NetPeer& peer,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                yuno::net::ByteReader r(body, bodyLen);
                (void)yuno::net::packets::C2S_MatchLeave::Deserialize(r);

                std::cout << "Leave :" << peer.sId << std::endl;

                const int idx = m_match.FindSlotBySessionId(peer.sId);
                if (idx >= 0)
                {
                    const auto& slots = m_match.Slots();
                    const auto uid = slots[static_cast<std::size_t>(idx)].userId;
                    m_match.LeaveByUserId(uid);
                }
            }
        );// Leave Packet End
        
        // ReadySet Packet Start
        m_dispatcher.RegisterRaw(
            PacketType::C2S_ReadySet,
            [this](const NetPeer& peer,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {

                yuno::net::ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::C2S_ReadySet::Deserialize(r);

                auto session = m_server.FindSession(peer.sId);
                if (!session) return;

                const bool isReady = (pkt.readyState != 0);
                if (!m_match.SetReadyBySessionId(peer.sId, isReady))
                    return;

                if (!isReady)
                {
                    m_countdownSent = false;
                }

                yuno::net::packets::S2C_ReadyState response{};
                response.p1_isReady = static_cast<std::uint8_t>(m_match.Slots()[0].ready ? 1 : 0);
                response.p2_isReady = static_cast<std::uint8_t>(m_match.Slots()[1].ready ? 1 : 0);

                auto bytes = PacketBuilder::Build(
                    PacketType::S2C_ReadyState,
                    [&](ByteWriter& w)
                    {
                        response.Serialize(w);
                    });

                m_server.Broadcast(std::move(bytes));
                //m_roundController.TryStartRound();
            }
        ); // ReadySet Packet End

        // Submit Weapon Packet Start
        m_dispatcher.RegisterRaw(
            PacketType::C2S_SubmitWeapon,
            [this](const NetPeer& peer,
                const PacketHeader&,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt = packets::C2S_SubmitWeapon::Deserialize(r);

                const int slotIdx =
                    m_match.FindSlotBySessionId(peer.sId);
                if (slotIdx < 0)
                    return;

                const auto& slot =
                    m_match.Slots()[slotIdx];

                // 1️ 무기 선택 상태 저장
                if (!m_match.SetUnitsByUserId(
                    slot.userId,
                    pkt.WeaponId1,
                    pkt.WeaponId2))
                    return;

                // 2️ 라운드 시작 조건 검사 (중앙 집중)
                m_roundController.TryStartRound();
            }
        );// Submit Weapon Packet End


        //Submit ReadyTurn Packet Start         
        m_dispatcher.RegisterRaw(
            PacketType::C2S_ReadyTurn,
            [this](const NetPeer& peer,
                const PacketHeader&,
                const uint8_t* body,
                uint32_t bodyLen)
            {
                ByteReader r(body, bodyLen);
                const auto pkt =
                    packets::C2S_ReadyTurn::Deserialize(r);

                for (auto& data : pkt.commands)
                {
                    std::cout << " data : " << data.runtimeID << " " << std::endl;
                }

                m_turnManager.SubmitTurn(
                    peer.sId,
                    pkt.commands);
            }
        );//Submit ReadyTurn Packet End
    }
}

