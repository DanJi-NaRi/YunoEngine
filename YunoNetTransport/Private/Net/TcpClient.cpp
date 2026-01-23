#include "pch.h"

#include "TcpClient.h"

#include <iostream>

namespace yuno::net
{
    TcpClient::TcpClient(boost::asio::io_context& io)
        : m_io(io)
        , m_resolver(io)
    {
    }

    void TcpClient::Connect(const std::string& host, std::uint16_t port)
    {
        if (m_session)
            return;

        std::cout << "[TcpClient] Resolve start: " << host << ":" << port << "\n";

        m_resolver.async_resolve(
            host,
            std::to_string(port),
            [this](const boost::system::error_code& ec,
                boost::asio::ip::tcp::resolver::results_type results)
            {
                std::cout << "[TcpClient] Resolve cb. ec=" << ec.message() << "\n";
                if (ec)
                {
                    std::cout << "[TcpClient] Resolve failed: " << ec.message() << "\n";
                    if (m_onDisconnected)
                        m_onDisconnected(ec);
                    return;
                }

                auto socket = std::make_shared<boost::asio::ip::tcp::socket>(m_io);

                boost::asio::async_connect(
                    *socket,
                    results,
                    [this, socket](const boost::system::error_code& ec,
                        const boost::asio::ip::tcp::endpoint& /*ep*/)
                    {
                        if (ec)
                        {
                            std::cout << "[TcpClient] Connect failed: " << ec.message() << "\n";
                            if (m_onDisconnected)
                                m_onDisconnected(ec);
                            return;
                        }

                        std::cout << "[TcpClient] Connect success\n";

                        constexpr sessionId kClientSid = 0;
                        auto session = std::make_shared<TcpSession>(kClientSid, std::move(*socket));
                        m_session = session;

                        HookSessionCallbacks(session);
                        session->Start();

                        if (m_onConnected)
                            m_onConnected();
                    }
                );
            }
        );
    }

    void TcpClient::Disconnect()
    {
        if (!m_session)
            return;

        m_session->Close();
        m_session.reset();
    }

    void TcpClient::Send(std::vector<std::uint8_t> packetBytes)
    {
        if (!m_session)
            return;

        m_session->Send(std::move(packetBytes));
    }

    void TcpClient::HookSessionCallbacks(const TcpSession::YunoSession& session)
    {
        session->SetOnPacket(
            [this](std::vector<std::uint8_t>&& pkt)
            {
                if (m_onPacket)
                    m_onPacket(std::move(pkt));
            });

        session->SetOnDisconnected(
            [this](const boost::system::error_code& ec)
            {
                auto old = m_session;
                m_session.reset();

                if (m_onDisconnected)
                    m_onDisconnected(ec);
            });
    }
}
