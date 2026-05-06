#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct C2S_Emote final
    {
        std::uint8_t emoteId;

        void Serialize(ByteWriter& w) const;
        static C2S_Emote Deserialize(ByteReader& r);
    };
}
