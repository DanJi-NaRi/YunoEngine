#include "pch.h"

#include "C2S_CardPackets.h"

namespace yuno::net::packets
{
    void C2S_SelectCard::Serialize(ByteWriter& w) const
    {
        w.WriteU32LE(runtimeID);
    }

    C2S_SelectCard C2S_SelectCard::Deserialize(ByteReader& r)
    {
        C2S_SelectCard pkt{};
        pkt.runtimeID = r.ReadU32LE();
        return pkt;
    }
}
