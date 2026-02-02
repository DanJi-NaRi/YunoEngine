#include "pch.h"

#include "TcpSession.h"

namespace yuno::net
{
    TcpSession::TcpSession(sessionId sid, boost::asio::ip::tcp::socket socket)
        : m_sid(sid)
        , m_socket(std::move(socket))
        , m_strand(m_socket.get_executor())
    {
    }

    void TcpSession::Start()
    {
        boost::asio::dispatch(m_strand, [self = shared_from_this()]()
            {
                self->ReadHeader();
            });
    }

    void TcpSession::Close()
    {
        boost::asio::dispatch(m_strand, [self = shared_from_this()]()
            {
                self->m_disconnectedNotified = true;

                boost::system::error_code ec;
                self->m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                self->m_socket.close(ec);
            });
    }

    void TcpSession::Send(std::vector<std::uint8_t> packetBytes)
    {
        boost::asio::dispatch(m_strand, [self = shared_from_this(), pkt = std::move(packetBytes)]() mutable
            {
                if (pkt.size() < yunoTCPPacketHeaderSize)
                    return;
                assert(pkt.size() >= yunoTCPPacketHeaderSize);

                const bool isEmpty = self->m_writeQ.empty();
                self->m_writeQ.push_back(std::move(pkt)); // 이때 패킷은 헤더 + 바디 전체임

                if (isEmpty && !self->m_writing)
                {
                    self->DoWrite();
                }
            });
    }

    void TcpSession::ReadHeader()
    {
        auto self = shared_from_this();

        boost::asio::async_read(
            m_socket,
            boost::asio::buffer(m_readHeader.data(), m_readHeader.size()),
            boost::asio::bind_executor(m_strand,
                [self](const boost::system::error_code& ec, std::size_t /*bytes*/)
                {
                    if (ec)
                    {
                        self->NotifyDisconnected(ec);
                        return;
                    }

                    const std::uint32_t bodyLen = TcpSession::ReadU32LE(self->m_readHeader.data());

                    
                    constexpr std::uint32_t MaxLength = 4u * 1024u * 1024u;
                    if (bodyLen > MaxLength)
                    {
                        boost::system::error_code fake = boost::asio::error::message_size;
                        self->NotifyDisconnected(fake);
                        return;
                    }

                    self->ReadBody(bodyLen);
                }
            )
        );
    }

    void TcpSession::ReadBody(std::uint32_t bodyLen)
    {
        auto self = shared_from_this();

        m_readBody.clear();
        m_readBody.resize(bodyLen);

        if (bodyLen == 0)
        {
            // 헤더만 있는 패킷도 허용
            std::vector<std::uint8_t> packet;
            packet.reserve(yunoTCPPacketHeaderSize);
            packet.insert(packet.end(), m_readHeader.begin(), m_readHeader.end());

            if (self->m_onPacket)
                self->m_onPacket(std::move(packet));

            self->ReadHeader();
            return;
        }

        boost::asio::async_read(
            m_socket,
            boost::asio::buffer(m_readBody.data(), m_readBody.size()),
            boost::asio::bind_executor(m_strand,
                [self](const boost::system::error_code& ec, std::size_t /*bytes*/)
                {
                    if (ec)
                    {
                        self->NotifyDisconnected(ec);
                        return;
                    }

                    // 완성 패킷(헤더8 + 바디N)을 한 덩어리로 올림
                    std::vector<std::uint8_t> packet;
                    packet.reserve(yunoTCPPacketHeaderSize + self->m_readBody.size());

                    packet.insert(packet.end(),
                        self->m_readHeader.begin(), self->m_readHeader.end());

                    packet.insert(packet.end(),
                        self->m_readBody.begin(), self->m_readBody.end());

                    if (self->m_onPacket)
                        self->m_onPacket(std::move(packet));

                    self->ReadHeader();
                }
            )
        );
    }

    void TcpSession::DoWrite()
    {
        if (m_writeQ.empty())
        {
            m_writing = false;
            return;
        }

        m_writing = true;
        auto self = shared_from_this();

        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(m_writeQ.front().data(), m_writeQ.front().size()),
            boost::asio::bind_executor(m_strand,
                [self](const boost::system::error_code& ec, std::size_t /*bytes*/)
                {
                    if (ec)
                    {
                        // 에러 발생하면 연결 끊김 알림
                        self->NotifyDisconnected(ec);
                        return;
                    }

                    self->m_writeQ.pop_front();
                    self->DoWrite();
                }
            )
        );
    }

    void TcpSession::NotifyDisconnected(const boost::system::error_code& ec)
    {
        if (m_disconnectedNotified)
            return;
        m_disconnectedNotified = true;

        // 콜백 먼저
        if (m_onDisconnected)
            m_onDisconnected(ec);

        // 소켓 닫기
        boost::system::error_code ignored;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
        m_socket.close(ignored);
    }

    std::uint32_t TcpSession::ReadU32LE(const std::uint8_t* p)
    {
        // p0가 LSB임 
        // 4Byte를 읽을거임
        // 리틀 엔디안
        // 0x000000[p0]
        // 0x0000[p1]00
        // 0x00[p2]0000
        // 0x[p3]000000

        return (std::uint32_t)p[0]              
            | ((std::uint32_t)p[1] << 8)
            | ((std::uint32_t)p[2] << 16)
            | ((std::uint32_t)p[3] << 24);
    }
}
