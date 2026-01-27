#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct S2C_ReadyState final
    {
        std::uint8_t p1_isReady = 0; // 0이면 준비해제, 1이면 준비완료
        std::uint8_t p2_isReady = 0;

        void Serialize(ByteWriter& w) const;

        static S2C_ReadyState Deserialize(ByteReader& r);
    };
}
