#include "pch.h"

#include "C2S_Emote.h"

namespace yuno::net::packets
{
    void C2S_Emote::Serialize(ByteWriter& w) const
    {
        w.WriteU8(emoteId);
    }

    C2S_Emote C2S_Emote::Deserialize(ByteReader& r)
    {
        C2S_Emote pkt{};
        pkt.emoteId = r.ReadU8();
        return pkt;
    }
}
