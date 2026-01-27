#include "YunoServerNetwork.h"

#include "PacketBuilder.h"

#include "C2S_Ping.h"
#include "C2S_MatchEnter.h"
#include "C2S_MatchLeave.h"
#include "C2S_SubmitWeapon.h"
#include "C2S_ReadySet.h"

#include "S2C_Pong.h"
#include "S2C_EnterOK.h"
#include "S2C_Error.h"
#include "S2C_CountDown.h"
#include "S2C_ReadyState.h"
#include "S2C_RoundStart.h"


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

            }
        ); // ReadySet Packet End

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

                if (!m_match.SetUnitsByUserId(uid, pkt.WeaponId1, pkt.WeaponId2))
                    return;

                const auto& s = m_match.Slots();

                const bool bothOccupied = (s[0].occupied && s[1].occupied);
                const bool bothReady = (s[0].ready && s[1].ready);
                const bool bothUnitsFilled = (s[0].IsUnitsFilled() && s[1].IsUnitsFilled());

                if (!(bothOccupied && bothReady && bothUnitsFilled))
                    return;

                if (m_countdownSent)
                    return;

                m_countdownSent = true;

                yuno::net::packets::S2C_CountDown cd{};
                cd.countTime = 3;
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

                m_server.Broadcast(std::move(bytes));


                std::cout <<"0slot user Env id : " << m_match.Slots()[0].userId << "1slot user Env id : "<< m_match.Slots()[1].userId << std::endl;

                // 무기 확정 후 준비 취소 못하니까  여기서 초기화 패킷도 같이 ㄱㄱ
                // 일단 체력, 스테미너는 기본값인데 추후 무기의 hp,스태미너 받아와서 넣기 ㄱㄱ
                yuno::net::packets::S2C_RoundStart rs{};
                {
                    // slot0
                    rs.units[0].PID = 1;
                    rs.units[0].slotID = 1;
                    rs.units[0].WeaponID = static_cast<std::uint8_t>(s[0].unitId1);
                    rs.units[0].hp = 100;          
                    rs.units[0].stamina = 100;     
                    rs.units[0].SpawnTileId = 9;  

                    rs.units[1].PID = 1;
                    rs.units[1].slotID = 2;
                    rs.units[1].WeaponID = static_cast<std::uint8_t>(s[0].unitId2);
                    rs.units[1].hp = 100;
                    rs.units[1].stamina = 100;
                    rs.units[1].SpawnTileId = 23;

                    // slot1
                    rs.units[2].PID = 2;
                    rs.units[2].slotID = 1;
                    rs.units[2].WeaponID = static_cast<std::uint8_t>(s[1].unitId1);
                    rs.units[2].hp = 100;
                    rs.units[2].stamina = 100;
                    rs.units[2].SpawnTileId = 13;

                    rs.units[3].PID = 2;
                    rs.units[3].slotID = 2;
                    rs.units[3].WeaponID = static_cast<std::uint8_t>(s[1].unitId2);
                    rs.units[3].hp = 100;
                    rs.units[3].stamina = 100;
                    rs.units[3].SpawnTileId = 27;
                }

                auto rsBytes = yuno::net::PacketBuilder::Build(
                    yuno::net::PacketType::S2C_RoundStart,
                    [&](yuno::net::ByteWriter& w)
                    {
                        rs.Serialize(w);
                    });

                m_server.Broadcast(std::move(rsBytes));

            }
        );// Submit Weapon Packet End

    }
}

