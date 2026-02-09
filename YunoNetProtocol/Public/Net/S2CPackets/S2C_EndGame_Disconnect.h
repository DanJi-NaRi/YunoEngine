#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct S2C_EndGame_Disconnect
    {
        uint8_t winnerPID;

        void Serialize(ByteWriter& w) const;
        static S2C_EndGame_Disconnect Deserialize(ByteReader& r);
    };
}
