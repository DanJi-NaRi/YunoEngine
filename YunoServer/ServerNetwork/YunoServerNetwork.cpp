#include "YunoServerNetwork.h"

#include "PacketBuilder.h"

#include "C2S_Ping.h"
#include "S2C_Pong.h"

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
            [](std::shared_ptr<yuno::net::TcpSession> /*session*/, const boost::system::error_code& /*ec*/)
            {
                // 필요하면 로깅/정리
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

        using namespace yuno::net;

        // === C2S_Ping 핸들러 바인딩 ===
        m_dispatcher.RegisterRaw(
            PacketType::C2S_Ping,
            [this](const NetPeer& peer,
                const PacketHeader& /*header*/,
                const std::uint8_t* body,
                std::uint32_t bodyLen)
            {
                std::cout << "[Server] C2S_Ping handler called. sid=" << peer.sId << "\n";

                if (bodyLen < 4)
                    return;

                ByteReader r(body, bodyLen);
                const auto ping = yuno::net::packets::C2S_Ping::Deserialize(r);

                yuno::net::packets::S2C_Pong pong{};
                pong.nonce = ping.nonce * 3;
                std::cout << "InComing Data : " << pong.nonce << std::endl;

                auto bytes = PacketBuilder::Build(
                    PacketType::S2C_Pong,
                    [&](ByteWriter& w)
                    {
                        pong.Serialize(w);
                    });

                auto session = m_server.FindSession(peer.sId);
                if (session)
                {
                    std::cout << "[Server] Send S2C_Pong sid=" << peer.sId << "\n";
                    session->Send(std::move(bytes));
                }
                else
                {
                    std::cout << "[Server] FindSession failed sid=" << peer.sId << "\n";
                }
            });

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

}
