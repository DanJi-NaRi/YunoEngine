#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct PlayerResult
    {
        uint8_t PID;
        uint8_t winCount;
    };

    struct S2C_EndGame
    {
        PlayerResult results[2];

        void Serialize(ByteWriter& w) const;
        static S2C_EndGame Deserialize(ByteReader& r);
    };
}
