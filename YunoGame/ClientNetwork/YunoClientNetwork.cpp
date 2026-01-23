#include "pch.h"

#include "YunoClientNetwork.h"

namespace yuno::game
{
    YunoClientNetwork::YunoClientNetwork()
        : m_workGuard(boost::asio::make_work_guard(m_io))
        , m_client(m_io)
    {
        m_serverPeer.sId = 0;

        m_client.SetOnPacket(
            [this](std::vector<std::uint8_t>&& packet)
            {
                // 이 콜백은 io_context 스레드에서 호출됨
                PushIncoming(std::move(packet));
            });

        m_client.SetOnDisconnected(
            [this](const boost::system::error_code& /*ec*/)
            {
                // 필요하면 "끊김 이벤트"를 별도 큐로도 올릴 수 있음
            });
    }

    YunoClientNetwork::~YunoClientNetwork()
    {
        Stop();
    }

    void YunoClientNetwork::Start(const std::string& host, std::uint16_t port)
    {
        if (m_running.exchange(true))
            return;

        // io_context 실행 스레드 시작
        m_netThread = std::thread(
            [this]()
            {
                m_io.run();
            });

        // 연결 시도는 네트워크 스레드 컨텍스트에서 실행되도록 post
        boost::asio::post(
            m_io,
            [this, host, port]()
            {
                m_client.Connect(host, port);
            });
    }

    void YunoClientNetwork::Stop()
    {
        if (!m_running.exchange(false))
            return;

        // Disconnect도 io 스레드에서 처리
        boost::asio::post(
            m_io,
            [this]()
            {
                m_client.Disconnect();
            });

        m_workGuard.reset();
        m_io.stop();

        if (m_netThread.joinable())
            m_netThread.join();

        // 큐 정리
        {
            std::lock_guard<std::mutex> lock(m_inMtx);
            m_inQ.clear();
        }
    }

    bool YunoClientNetwork::IsConnected() const
    {
        return m_client.IsConnected();
    }

    void YunoClientNetwork::SendPacket(std::vector<std::uint8_t> packetBytes)
    {
        boost::asio::post(
            m_io,
            [this, bytes = std::move(packetBytes)]() mutable
            {
                m_client.Send(std::move(bytes));
            });
    }

    void YunoClientNetwork::PumpIncoming()
    {
        // 메인 스레드에서만 호출
        std::vector<std::uint8_t> pkt;
        while (PopIncoming(pkt))
        {
            m_dispatcher.Dispatch(m_serverPeer, pkt);
        }
    }

    void YunoClientNetwork::PushIncoming(std::vector<std::uint8_t>&& packetBytes)
    {
        std::lock_guard<std::mutex> lock(m_inMtx);
        m_inQ.emplace_back(std::move(packetBytes));
    }

    bool YunoClientNetwork::PopIncoming(std::vector<std::uint8_t>& out)
    {
        std::lock_guard<std::mutex> lock(m_inMtx);
        if (m_inQ.empty())
            return false;

        out = std::move(m_inQ.front());
        m_inQ.pop_front();
        return true;
    }
}
