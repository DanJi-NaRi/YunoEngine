#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct S2C_Emote final
    {
        std::uint8_t PID;           //누가 보냈는지 P1 P2
        std::uint8_t emoteId;    //1 2 3

        void Serialize(ByteWriter& w) const;
        static S2C_Emote Deserialize(ByteReader& r);
    };
}
