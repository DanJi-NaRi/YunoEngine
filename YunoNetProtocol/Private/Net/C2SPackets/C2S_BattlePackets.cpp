#include "pch.h"

#include "C2S_BattlePackets.h"

namespace yuno::net::packets
{
    void C2S_ReadyTurn::Serialize(ByteWriter& w) const
    {
        w.WriteU16LE(static_cast<std::uint16_t>(runtimeIDs.size()));
        for (auto id : runtimeIDs)
        {
            w.WriteU32LE(id);
        }
    }

    C2S_ReadyTurn C2S_ReadyTurn::Deserialize(ByteReader& r)
    {
        C2S_ReadyTurn pkt;

        const std::uint16_t count = r.ReadU16LE();
        pkt.runtimeIDs.reserve(count);

        for (std::uint16_t i = 0; i < count; ++i)
        {
            pkt.runtimeIDs.push_back(r.ReadU32LE());
        }

        return pkt;
    }
}
