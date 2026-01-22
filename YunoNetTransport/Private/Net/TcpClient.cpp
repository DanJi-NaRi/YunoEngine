#include "pch.h"

#include "TcpClient.h"

namespace yuno::net
{
    TcpClient::TcpClient(boost::asio::io_context& io)
        : m_io(io)
        , m_resolver(io)
    {
    }

    void TcpClient::Connect(const std::string& host, std::uint16_t port)
    {
        // 이미 연결된 상태면 무시(정책)
        if (m_session)
            return;

        // host:port resolve
        m_resolver.async_resolve(
            host,
            std::to_string(port),
            [this](const boost::system::error_code& ec,
                boost::asio::ip::tcp::resolver::results_type results)
            {
                if (ec)
                {
                    // resolve 실패를 disconnected로 보고 싶으면 여기서 콜백 호출 가능
                    if (m_onDisconnected)
                        m_onDisconnected(ec);
                    return;
                }

                boost::asio::ip::tcp::socket socket(m_io);

                boost::asio::async_connect(
                    socket,
                    results,
                    [this, socket = std::move(socket)](const boost::system::error_code& ec,
                        const boost::asio::ip::tcp::endpoint& /*ep*/) mutable
                    {
                        if (ec)
                        {
                            if (m_onDisconnected)
                                m_onDisconnected(ec);
                            return;
                        }

                        auto session = std::make_shared<TcpSession>(std::move(socket));
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

        // 정상 종료(콜백 X 정책). TcpSession::Close()가 OnDisconnected를 막음.
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
                // 에러/상대 종료로 인한 끊김만 여기로 들어온다(정상 Disconnect는 Close가 막음)
                auto old = m_session;
                m_session.reset();

                if (m_onDisconnected)
                    m_onDisconnected(ec);
            });
    }
}
