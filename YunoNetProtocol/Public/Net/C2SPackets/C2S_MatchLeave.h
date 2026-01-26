#pragma once

namespace yuno::net::packets
{
    struct C2S_MatchLeave final
    {
        void Serialize(ByteWriter& w) const;
        static C2S_MatchLeave Deserialize(ByteReader& r);
    };
}
