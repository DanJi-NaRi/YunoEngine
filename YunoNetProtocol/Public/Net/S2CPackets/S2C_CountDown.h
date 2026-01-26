#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct S2C_CountDown final
    {
        std::uint8_t countTime = 0;

        std::uint8_t slot1_UnitId1 = 0; // 유닛 범위는 1~6, 0은 error
        std::uint8_t slot1_UnitId2 = 0; // 유닛 범위는 1~6, 0은 error

        std::uint8_t slot2_UnitId1 = 0; // 유닛 범위는 1~6, 0은 error
        std::uint8_t slot2_UnitId2 = 0; // 유닛 범위는 1~6, 0은 error

        void Serialize(ByteWriter& w) const;
        static S2C_CountDown Deserialize(ByteReader& r);
    };
}
