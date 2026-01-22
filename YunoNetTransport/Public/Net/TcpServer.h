#pragma once

#include "TcpSession.h"

namespace yuno::net
{
    class TcpServer final
    {
    public:
        using OnPacketFn =
            std::function<void(TcpSession::YunoSession session, std::vector<std::uint8_t>&& packet)>;

        using OnDisconnectedFn =
            std::function<void(TcpSession::YunoSession session, const boost::system::error_code& ec)>;

    public:
        explicit TcpServer(boost::asio::io_context& io);

        TcpServer(const TcpServer&) = delete;
        TcpServer& operator=(const TcpServer&) = delete;

        // 서버 시작(바인드+리스닝)
        bool Start(std::uint16_t port, std::size_t backlog = boost::asio::socket_base::max_listen_connections);

        // 서버 정지(accept 중단 + 모든 세션 종료)
        void Stop();

        bool IsRunning() const { return m_running; }

        void SetOnPacket(OnPacketFn fn) { m_onPacket = std::move(fn); }
        void SetOnDisconnected(OnDisconnectedFn fn) { m_onDisconnected = std::move(fn); }

        // 브로드캐스트(현재 접속 중인 모든 세션)
        void Broadcast(std::vector<std::uint8_t> packetBytes);

        std::size_t GetSessionCount() const { return m_sessions.size(); }

    private:
        void DoAccept();

        // 세션 콜백 연결
        void HookSessionCallbacks(const TcpSession::YunoSession& session);

        // 세션 제거
        void RemoveSession(std::uint64_t sessionId);

        // 단조 증가 세션 ID
        std::uint64_t NextSessionId() { return ++m_nextSessionId; }

    private:
        boost::asio::io_context& m_io;
        boost::asio::ip::tcp::acceptor m_acceptor;

        bool m_running = false;

        std::uint64_t m_nextSessionId = 0;

        // sessionId -> session
        std::unordered_map<std::uint64_t, TcpSession::YunoSession> m_sessions;

        OnPacketFn m_onPacket;
        OnDisconnectedFn m_onDisconnected;
    };
}
