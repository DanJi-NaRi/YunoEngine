#include "pch.h"

#include "C2S_MatchEnter.h"

namespace yuno::net::packets
{
    void C2S_MatchEnter::Serialize(ByteWriter& w) const
    {
    }

    C2S_MatchEnter C2S_MatchEnter::Deserialize(ByteReader& r)
    {
        return {};
    }
}
