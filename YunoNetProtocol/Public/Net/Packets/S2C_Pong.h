#pragma once


namespace yuno::net::packets
{
    struct S2C_Pong final
    {
        std::uint32_t nonce = 0;

        void Serialize(ByteWriter& w) const;


        static S2C_Pong Deserialize(ByteReader& r);

    };
}
