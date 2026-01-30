#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct C2S_ReadyTurn final
    {
        std::vector<std::uint32_t> runtimeIDs;

        void Serialize(ByteWriter& w) const;
        static C2S_ReadyTurn Deserialize(ByteReader& r);
    };
}
