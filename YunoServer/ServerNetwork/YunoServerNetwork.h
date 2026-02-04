#pragma once

#include <cstdint>
#include <memory>

#include "TcpServer.h"          // YunoNetTransport
#include "PacketDispatcher.h"   // YunoNetProtocol
#include "NetPeer.h"            // YunoNetProtocol
#include "MatchManager.h"
#include "ServerCardRuntime.h"
#include "ServerCardManager.h"
#include "ServerCardRangeManager.h"
#include "RoundController.h"
#include "ServerCardDealer.h"
#include "TurnManager.h"
#include "PlayerCardController.h"
#include "BattleState.h"

namespace yuno::net
{
    class TcpSession;
}

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

        // 래퍼
    public:
        void Broadcast(std::vector<std::uint8_t>&& bytes);

        std::shared_ptr<yuno::net::TcpSession> FindSession(std::uint64_t sessionId);
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
        bool m_countdownSent = false;
        
        ServerCardDealer  m_cardDealer;
        ServerCardManager m_cardDB;
        ServerCardRangeManager m_cardRangeDB;
        ServerCardRuntime m_cardRuntime;
        RoundController m_roundController;
        TurnManager m_turnManager;
        PlayerCardController m_cardController;
    };
}
