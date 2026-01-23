#include "pch.h"

#include "PacketDispatcher.h"

namespace yuno::net
{
    // 패킷 타입과 람다 함수 넣으면 디스패처 클래스 멤버 맵에 저장됨
    // 게임과 서버에서 패킷 타입에 따라 어떻게 다룰지 등록하면 됨
    void PacketDispatcher::RegisterRaw(PacketType type, RawHandler handler)
    {
        if (!handler)
        {
            m_rawHandlers.erase(type);
            return;
        }

        m_rawHandlers[type] = std::move(handler);
    }

    void PacketDispatcher::Unregister(PacketType type)
    {
        m_rawHandlers.erase(type);
    }

    bool PacketDispatcher::Dispatch(const std::vector<std::uint8_t>& packetBytes)
    {
        // 패킷 사이즈가 헤더보다 작으면 뭔가 잘못된 패킷임
        if (packetBytes.size() < yunoPacketHeaderSize)
            return false;

        const PacketHeader h = UnPackHeaderLE(packetBytes.data());

        if (!ValidateHeaderAndSize(h, packetBytes.size()))
            return false;

        // 방향 검증 
        // System 패킷은 양쪽 다 가능
        // S2C는 Client만 받음
        // C2S는 Server만 받음
        if (!ValidateDirection(h.type))
            return false;

        const std::uint8_t* body = packetBytes.data() + yunoPacketHeaderSize;
        const std::uint32_t bodyLen = h.bodyLength;

        auto it = m_rawHandlers.find(h.type);
        if (it == m_rawHandlers.end())
        {  
            // 등록 안 된 타입의 패킷이면 버림
            return false;
        }

        it->second(h, body, bodyLen);
        return true;
    }

    bool PacketDispatcher::ValidateHeaderAndSize(const PacketHeader& h, std::size_t packetSize) const
    {
        if (!IsValidPacketType(h.type))
            return false;

        // Body 길이가 너무 크면 뭔가 잘못된 패킷임
        if (!IsHeaderBodyLengthSane(h.bodyLength))
            return false;

        const std::size_t expected = yunoPacketHeaderSize + static_cast<std::size_t>(h.bodyLength);
        if (packetSize != expected)
            return false;

        return true;
    }

    bool PacketDispatcher::ValidateDirection(PacketType t) const
    {
        if (IsSystem(t))
            return true;

        switch (m_role)
        {
        case EndpointRole::Any:
            return true;

        case EndpointRole::Server:
            // 서버는 C2S만 받음
            return IsC2S(t);

        case EndpointRole::Client:
            // 클라는 S2C만 받음
            return IsS2C(t);

        default:
            return false;
        }
    }
}
