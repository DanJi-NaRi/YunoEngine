#include "pch.h"

#include "C2S_MatchLeave.h"

namespace yuno::net::packets
{
    void C2S_MatchLeave::Serialize(ByteWriter& w) const
    {
    }

    C2S_MatchLeave C2S_MatchLeave::Deserialize(ByteReader& r)
    {
        return {};
    }
}
