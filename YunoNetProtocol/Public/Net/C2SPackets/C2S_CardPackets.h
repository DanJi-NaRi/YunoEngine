#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct C2S_SelectCard final
    {
        uint32_t runtimeID;

        void Serialize(ByteWriter& w) const;
        static C2S_SelectCard Deserialize(ByteReader& r);
    };
}
