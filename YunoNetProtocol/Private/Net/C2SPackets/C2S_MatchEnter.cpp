#include "pch.h"

#include "C2S_MatchEnter.h"

namespace yuno::net::packets
{
    void C2S_MatchEnter::Serialize(ByteWriter& w) const
    {
        w.WriteU32LE(userId);
    }

    C2S_MatchEnter C2S_MatchEnter::Deserialize(ByteReader& r)
    {
        C2S_MatchEnter pkt{};
        pkt.userId = r.ReadU32LE();
        return pkt;
    }
}
