#include "pch.h"
#include "PacketBuilder.h"

#include "ByteIO.h"
#include "PacketHeader.h"

namespace yuno::net
{

    // 패킷 만들어주는 놈
    std::vector<std::uint8_t> PacketBuilder::Build(PacketType type, BuildBodyFn buildBody,
        std::uint8_t version, std::uint16_t reserved)
    {

        std::vector<std::uint8_t> out;
        out.resize(yunoPacketHeaderSize); // 헤더 8바이트 미리 확보

        {
            ByteWriter w(out);
            if (buildBody)
                buildBody(w);
        }

        const std::uint32_t bodyLen =
            static_cast<std::uint32_t>(out.size() - yunoPacketHeaderSize);

        const auto header = PackHeaderLE(bodyLen, type, version, reserved);
        std::memcpy(out.data(), header.data(), yunoPacketHeaderSize);

        return out;
    }
}


// 사용하는 곳은 타입별로 패킷을 만드는 곳
/*
auto bytes = yuno::net::PacketBuilder::Build(
    yuno::net::PacketType::C2S_MatchEnter,
    [](yuno::net::ByteWriter& w)
    {
        w.WriteU32LE(data1);
        w.WriteU16LE(data2);
        w.WriteU8(data3); 이렇게 원하는 데이터 넣으면 됨
    }
);

만들고나서 send하면 보내짐
client.Send(std::move(bytes));
*/
