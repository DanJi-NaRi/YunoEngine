#include "pch.h"
#include "TcpSession.h"   
#include "TcpServer.h"
#include <iostream>

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

        for (auto& sess : m_sessions)
        {
            if (sess.second)
                sess.second->Close();
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
                    DoAccept();
                    return;
                }
                //std::cout << "[TcpServer] accepted\n";

                // 서버
                const sessionId sid = NextSessionId();
                auto session = std::make_shared<TcpSession>(sid, std::move(socket));
                m_sessions.emplace(sid, session);

                std::cout << "[TcpServer] new session sid=" << sid << "\n";

                HookSessionCallbacks(sid,session);
                session->Start();

                DoAccept();
            }
        );
    }

    void TcpServer::HookSessionCallbacks(sessionId sid,  const TcpSession::YunoSession& session)
    {
        // 패킷 수신
        // shared로 하면 순환 참조돼서 해제가 안됨!!
        session->SetOnPacket(
            [this, weak = std::weak_ptr<TcpSession>(session)](std::vector<std::uint8_t>&& pkt)
            {
                auto s = weak.lock();
                if (!s) return;

                if (m_onPacket)
                    m_onPacket(s, std::move(pkt));
            });

        // 끊김(에러/상대 종료)
        // shared로 하면 순환 참조돼서 해제가 안됨!!
        session->SetOnDisconnected(
            [this, sid, weak = std::weak_ptr<TcpSession>(session)](const boost::system::error_code& ec)
            {
                auto s = weak.lock(); 
                if (!s) return;

                RemoveSession(sid);

                if (m_onDisconnected)
                    m_onDisconnected(s, ec);
            });
    }

    void TcpServer::RemoveSession(sessionId id)
    {
        auto it = m_sessions.find(id);
        if (it == m_sessions.end())
            return;

        m_sessions.erase(it);
    }

    void TcpServer::Broadcast(std::vector<std::uint8_t> packetBytes)
    {

        for (auto& sess : m_sessions)
        {
            if (!sess.second)
                continue;

            sess.second->Send(packetBytes);
        }
    }

    TcpSession::YunoSession TcpServer::FindSession(sessionId sid) const
    {
        auto it = m_sessions.find(sid);
        if (it == m_sessions.end())
            return nullptr;
        return it->second;
    }

}
