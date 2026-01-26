#pragma once

namespace yuno::net::packets
{
    struct C2S_MatchEnter final
    {
        std::uint32_t userId = 0;

        void Serialize(ByteWriter& w) const;
        static C2S_MatchEnter Deserialize(ByteReader& r);
    };
}
