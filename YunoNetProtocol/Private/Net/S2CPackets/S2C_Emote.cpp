#include "pch.h"

#include "S2C_Emote.h"

namespace yuno::net::packets
{
    void S2C_Emote::Serialize(ByteWriter& w) const
    {
        w.WriteU8(PID);
        w.WriteU8(emoteId);
    }

    S2C_Emote S2C_Emote::Deserialize(ByteReader& r)
    {
        S2C_Emote pkt{};
        pkt.PID = r.ReadU8();
        pkt.emoteId = r.ReadU8();
        return pkt;
    }
}
