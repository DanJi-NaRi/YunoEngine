#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct S2C_TestCardList
    {
        std::vector<std::uint32_t> runtimeIDs;

        void Serialize(ByteWriter& w) const;
        static S2C_TestCardList Deserialize(ByteReader& r);
    };
}
