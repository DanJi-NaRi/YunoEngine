#pragma once

#include <cstdint>
#include <memory>

#include "TcpServer.h"          // YunoNetTransport
#include "PacketDispatcher.h"   // YunoNetProtocol
#include "NetPeer.h"            // YunoNetProtocol
#include "MatchManager.h"

namespace yuno::server
{
    class YunoServerNetwork final
    {
    public:
        YunoServerNetwork();
        ~YunoServerNetwork();

        YunoServerNetwork(const YunoServerNetwork&) = delete;
        YunoServerNetwork& operator=(const YunoServerNetwork&) = delete;

        bool Start(std::uint16_t port);
        void Tick();
        void Stop();

        yuno::net::PacketDispatcher& Dispatcher() { return m_dispatcher; }

        // 핸들러 등록
    public:
        void RegisterMatchPacketHandler();

    private:
        boost::asio::io_context m_io;
        yuno::net::TcpServer m_server;
        yuno::net::PacketDispatcher m_dispatcher{ yuno::net::PacketDispatcher::EndpointRole::Server };
    
        // 매치 데이터
    private:
        MatchManager m_match;

    };
}
