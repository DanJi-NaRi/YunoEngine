#pragma once



namespace yuno::net::packets
{
    struct C2S_Ping final
    {
        std::uint32_t nonce = 0;

        void Serialize(ByteWriter& w) const;
        static C2S_Ping Deserialize(ByteReader& r);
    };
}
