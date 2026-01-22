#include "pch.h"
#include "TcpServer.h"

namespace yuno::net
{
    TcpServer::TcpServer(boost::asio::io_context& io)
        : m_io(io)
        , m_acceptor(io)
    {
    }

    bool TcpServer::Start(std::uint16_t port, std::size_t backlog)
    {
        if (m_running)
            return true;

        boost::system::error_code ec;

        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);

        m_acceptor.open(ep.protocol(), ec);
        if (ec) return false;

        m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
        if (ec) return false;

        m_acceptor.bind(ep, ec);
        if (ec) return false;

        m_acceptor.listen(static_cast<int>(backlog), ec);
        if (ec) return false;

        m_running = true;
        DoAccept();
        return true;
    }

    void TcpServer::Stop()
    {
        if (!m_running)
            return;

        m_running = false;

        boost::system::error_code ec;
        m_acceptor.cancel(ec);
        m_acceptor.close(ec);

        // 모든 세션 정상 종료(콜백 X 정책)
        for (auto& kv : m_sessions)
        {
            if (kv.second)
                kv.second->Close();
        }
        m_sessions.clear();
    }

    void TcpServer::DoAccept()
    {
        if (!m_running)
            return;

        m_acceptor.async_accept(
            [this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
            {
                if (!m_running)
                    return;

                if (ec)
                {
                    // acceptor가 닫히는 과정에서 operation_aborted가 올 수 있음
                    // running 상태면 다음 accept 계속
                    DoAccept();
                    return;
                }

                auto session = std::make_shared<TcpSession>(std::move(socket));
                const std::uint64_t sid = NextSessionId();
                m_sessions.emplace(sid, session);

                HookSessionCallbacks(session);
                session->Start();

                DoAccept();
            }
        );
    }

    void TcpServer::HookSessionCallbacks(const TcpSession::YunoSession& session)
    {
        // 패킷 수신
        session->SetOnPacket(
            [this, weak = std::weak_ptr<TcpSession>(session)](std::vector<std::uint8_t>&& pkt)
            {
                auto s = weak.lock();
                if (!s) return;

                if (m_onPacket)
                    m_onPacket(s, std::move(pkt));
            });

        // 끊김(에러/상대 종료)
        session->SetOnDisconnected(
            [this, weak = std::weak_ptr<TcpSession>(session)](const boost::system::error_code& ec)
            {
                auto s = weak.lock();
                if (!s) return;

                // 세션 제거: 현재 session 포인터로 id를 찾는 대신,
                // 맵을 스캔해서 제거(세션 수가 많아지면 개선 가능)
                std::uint64_t foundId = 0;
                for (const auto& kv : m_sessions)
                {
                    if (kv.second == s)
                    {
                        foundId = kv.first;
                        break;
                    }
                }
                if (foundId != 0)
                    RemoveSession(foundId);

                if (m_onDisconnected)
                    m_onDisconnected(s, ec);
            });
    }

    void TcpServer::RemoveSession(std::uint64_t sessionId)
    {
        auto it = m_sessions.find(sessionId);
        if (it == m_sessions.end())
            return;

        // Remove 시에도 Close는 불필요(이미 disconnect로 들어온 상황)
        m_sessions.erase(it);
    }

    void TcpServer::Broadcast(std::vector<std::uint8_t> packetBytes)
    {
        // 각 세션 Send가 내부에서 pkt를 move 해서 큐에 넣으므로,
        // 여기서는 복사가 불가피. (필요하면 shared buffer로 최적화 가능)
        for (auto& kv : m_sessions)
        {
            if (!kv.second)
                continue;

            kv.second->Send(packetBytes);
        }
    }
}
