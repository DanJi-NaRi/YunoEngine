#pragma once

#define sessionId std::uint64_t

namespace yuno::net
{
    // 4 BodyLen, 1 PacketType, 1 Version, 2 Reserved
    static constexpr std::size_t yunoPacketHeaderSize = 8;

    // 전송 계층 TCP 세션 클래스
    class TcpSession final : public std::enable_shared_from_this<TcpSession>
    {
    public:
        // 게임이랑 서버에서 사용할 별칭
        using YunoSession = std::shared_ptr<TcpSession>;

        // 패킷 수신 콜백
        using OnPacketFn = std::function<void(std::vector<std::uint8_t>&& packet)>;

        // 연결 끊김 콜백
        using OnDisconnectedFn = std::function<void(const boost::system::error_code& ec)>;

    public:
        explicit TcpSession(boost::asio::ip::tcp::socket socket);

        TcpSession(const TcpSession&) = delete;
        TcpSession& operator=(const TcpSession&) = delete;

        void Start();   // 수신 루프 시작 >> socket연결 완료 후에 호출할 것!!
        void Close();   // 세션 종료 >> 서버는 Shutdown시 , 클라이언트는 종료 시 호출할 것!!

        // 패킷 전송 이 때 패킷은 헤더 + 바디 전체를 의미
        void Send(std::vector<std::uint8_t> packetBytes);   

        void SetOnPacket(OnPacketFn fn) { m_onPacket = std::move(fn); }
        void SetOnDisconnected(OnDisconnectedFn fn) { m_onDisconnected = std::move(fn); }

        boost::asio::ip::tcp::socket& Socket() { return m_socket; }

    private:
        // 우리 헤더 규약인 패킷을 8Byte만 읽는 함수
        void ReadHeader();
        // 헤더에서 읽은 BodyLength 만큼 패킷을 읽는 함수
        // bodyLength가 0이면 헤더만 있는 패킷임
        void ReadBody(std::uint32_t bodyLength);
        // 패킷 전송 함수
        void DoWrite();

        // 에러나면 호출해서 
        void NotifyDisconnected(const boost::system::error_code& ec);

        // 리틀 엔디안 사용
        static std::uint32_t ReadU32LE(const std::uint8_t* p);

    private:
        boost::asio::ip::tcp::socket m_socket;
        boost::asio::strand<boost::asio::any_io_executor> m_strand;

        std::array<std::uint8_t, yunoPacketHeaderSize> m_readHeader{};
        std::vector<std::uint8_t> m_readBody;

        std::deque<std::vector<std::uint8_t>> m_writeQ;
        bool m_writing = false;

        OnPacketFn m_onPacket;
        OnDisconnectedFn m_onDisconnected;
        bool m_disconnectedNotified = false;
    };
}
