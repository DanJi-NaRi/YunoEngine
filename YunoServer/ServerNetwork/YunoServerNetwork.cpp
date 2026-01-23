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
                std::cout << "[Server] OnPacket: sid=" << session->GetSessionId()
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
