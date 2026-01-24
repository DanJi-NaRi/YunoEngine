#pragma once

#include "NetPeer.h"
#include "PacketHeader.h"
#include "PacketType.h"

namespace yuno::net
{
    // Transport에서 올라온 "완성 패킷(헤더+바디)"을 파싱해서
    // 타입별로 등록된 핸들러를 호출한다.
    class PacketDispatcher final
    {
    public:
        using RawHandler = std::function<void(
            const NetPeer& peer,
            const PacketHeader& header,
            const std::uint8_t* body,
            std::uint32_t bodyLen)>;

    public:

        // Any    : 방향 검증 안 함
        // Server : 수신 타입이 C2S 또는 System만 허용
        // Client : 수신 타입이 S2C 또는 System만 허용
        enum class EndpointRole : std::uint8_t
        {
            Any = 0,
            Server,
            Client
        };

    public:
        explicit PacketDispatcher(EndpointRole role = EndpointRole::Any)
            : m_role(role)
        {
        }

        void SetRole(EndpointRole role) { m_role = role; }

        // 타입별 raw 핸들러 등록
        void RegisterRaw(PacketType type, RawHandler handler);

        // 등록 해제
        void Unregister(PacketType type);


        //bool Dispatch(const std::vector<std::uint8_t>& packetBytes);
        bool Dispatch(const NetPeer& peer, const std::vector<std::uint8_t>& packetBytes);

    private:
        // 패킷 최소 사이즈는 헤더와 같거나 커야 됨
        bool ValidateHeaderAndSize(const PacketHeader& h, std::size_t packetSize) const;
        // 서버가 S2C 받거나 , 클라가 C2S 받는 상황 방지
        bool ValidateDirection(PacketType t) const;

    private:
        EndpointRole m_role = EndpointRole::Any;
        std::unordered_map<PacketType, RawHandler> m_rawHandlers;
    };
}
