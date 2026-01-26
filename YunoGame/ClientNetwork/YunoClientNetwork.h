#pragma once

#include <boost/asio.hpp>

#include "TcpClient.h"          // YunoNetTransport
#include "PacketDispatcher.h"   // YunoNetProtocol
#include "NetPeer.h"            // YunoNetProtocol


namespace yuno::game
{
    // 게임 메인 <-> 네트워크 스레드 연결용 래퍼
    class YunoClientNetwork final
    {
    public:
        YunoClientNetwork();
        ~YunoClientNetwork();

        YunoClientNetwork(const YunoClientNetwork&) = delete;
        YunoClientNetwork& operator=(const YunoClientNetwork&) = delete;

        void Start(const std::string& host, std::uint16_t port);
        void Stop();

        bool IsConnected() const;

        // 서버한테 받은 패킷을 디스패쳐로 전달
        void PumpIncoming();

        // 메인 스레드에서 호출: "완성 패킷(헤더+바디)" 바이트를 송신 요청
        void SendPacket(std::vector<std::uint8_t> packetBytes);

        // 게임에서 핸들러 등록할 수 있게 dispatcher 접근 제공
        yuno::net::PacketDispatcher& Dispatcher() { return m_dispatcher; }

        // 핸들러 등록
    public:
        void RegisterMatchPacketHandler();

    private:
        void PushIncoming(std::vector<std::uint8_t>&& packetBytes);
        bool PopIncoming(std::vector<std::uint8_t>& out);

    private:
        // --- Network thread members ---
        boost::asio::io_context m_io;
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;
        std::thread m_netThread;

        yuno::net::TcpClient m_client;

        std::atomic<bool> m_running{ false };

        // --- Main thread processing ---
        yuno::net::PacketDispatcher m_dispatcher{ yuno::net::PacketDispatcher::EndpointRole::Client };
        yuno::net::NetPeer m_serverPeer{}; // client는 서버 peer를 하나로 취급 (sId=0으로 시작)

        // --- Incoming queue (net thread -> main thread) ---
        std::mutex m_inMtx;
        std::deque<std::vector<std::uint8_t>> m_inQ;
    };
}

/*



*/
