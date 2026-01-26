#include "YunoServerNetwork.h"

#include "PacketBuilder.h"

#include "C2S_Ping.h"
#include "C2S_MatchEnter.h"
#include "C2S_MatchLeave.h"
#include "C2S_SubmitWeapon.h"

#include "S2C_Pong.h"
#include "S2C_EnterOK.h"
#include "S2C_Error.h"
#include "S2C_CountDown.h"


#include <iostream>
namespace yuno::server
{
    YunoServerNetwork::YunoServerNetwork()
        : m_io()
        , m_server(m_io)  
    {


        m_server.SetOnPacket(
            [this](std::shared_ptr<yuno::net::TcpSession> session, std::vector<std::uint8_t>&& packetBytes)
            {
                std::cout << "---------------------------[Server] OnPacket: sid=" << session->GetSessionId()
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
                    ok.slotIndex = static_cast<std::uint8_t>(slotIdx);
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
        

        // Submit Weapon Packet Start
        m_dispatcher.RegisterRaw(
            PacketType::C2S_SubmitWeapon,
            [this](const NetPeer& peer,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                yuno::net::ByteReader r(body, bodyLen);
                const auto pkt = yuno::net::packets::C2S_SubmitWeapon::Deserialize(r);

                const int slotIdx = m_match.FindSlotBySessionId(peer.sId);
                if (slotIdx < 0)
                    return;

                const auto& slots = m_match.Slots();
                const std::uint32_t uid = slots[slotIdx].userId;
                if (uid == 0)
                    return;

                const bool ok = m_match.SetUnitsByUserId(uid, pkt.WeaponId1, pkt.WeaponId1);
                if (!ok)
                    return;

                const auto& s = m_match.Slots();

                // 슬롯이 2명 다 차 있고 + 둘 다 unit 2개가 채워졌을 때만
                const bool bothOccupied = (s[0].occupied && s[1].occupied);
                const bool bothReady = (s[0].IsReady() && s[1].IsReady());

                if (!(bothOccupied && bothReady))
                {
                    // 둘 중 한명이 제출 안함
                    return;
                }

                yuno::net::packets::S2C_CountDown cd{};
                cd.countTime = 3;  // 3 초
                cd.slot1_UnitId1 = static_cast<uint8_t>(s[0].unitId1);
                cd.slot1_UnitId2 = static_cast<uint8_t>(s[0].unitId2);
                cd.slot2_UnitId1 = static_cast<uint8_t>(s[1].unitId1);
                cd.slot2_UnitId2 = static_cast<uint8_t>(s[1].unitId2);

                auto bytes = yuno::net::PacketBuilder::Build(
                    yuno::net::PacketType::S2C_CountDown,
                    [&](yuno::net::ByteWriter& w)
                    {
                        cd.Serialize(w);
                    });

                m_server.Broadcast(std::move(bytes));

            }
        );// Submit Weapon Packet End

    }
}

