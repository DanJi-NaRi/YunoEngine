#pragma once

#include "TcpSession.h"


// 소유 관계 정리
/*
TCPClient
ㄴ m_session (TCPSession)
    ㄴ TCPSession
        ㄴ async_read/write 콜백 소유
*/


namespace yuno::net
{
    class TcpClient final
    {
    public:
        using OnConnectedFn = std::function<void()>;
        using OnPacketFn = std::function<void(std::vector<std::uint8_t>&& packet)>;
        using OnDisconnectedFn = std::function<void(const boost::system::error_code& ec)>;

    public:
        explicit TcpClient(boost::asio::io_context& io);

        TcpClient(const TcpClient&) = delete;
        TcpClient& operator=(const TcpClient&) = delete;

        // 비동기 연결 시작 (host는 "127.0.0.1" 같은 문자열)
        void Connect(const std::string& host, std::uint16_t port);

        void Disconnect();

        bool IsConnected() const { return (m_session != nullptr); }

        void Send(std::vector<std::uint8_t> packetBytes);

        void SetOnConnected(OnConnectedFn fn) { m_onConnected = std::move(fn); }
        void SetOnPacket(OnPacketFn fn) { m_onPacket = std::move(fn); }
        void SetOnDisconnected(OnDisconnectedFn fn) { m_onDisconnected = std::move(fn); }

        TcpSession::YunoSession GetSession() const { return m_session; }

    private:
        void HookSessionCallbacks(const TcpSession::YunoSession& session);

    private:
        boost::asio::io_context& m_io;
        boost::asio::ip::tcp::resolver m_resolver;

        TcpSession::YunoSession m_session;

        OnConnectedFn m_onConnected;
        OnPacketFn m_onPacket;
        OnDisconnectedFn m_onDisconnected;
    };
}
