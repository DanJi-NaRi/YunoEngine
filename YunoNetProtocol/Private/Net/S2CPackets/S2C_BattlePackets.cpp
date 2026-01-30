#include "pch.h"

#include "S2C_BattlePackets.h"

namespace yuno::net::packets
{
    // UnitStateDelta
    void UnitStateDelta::Serialize(ByteWriter& w) const
    {
        w.WriteU8(ownerSlot);
        w.WriteU8(unitLocalIndex);

        w.WriteU32LE(static_cast<uint32_t>(hpDelta));
        w.WriteU32LE(static_cast<uint32_t>(staminaDelta));
        w.WriteU32LE(static_cast<uint32_t>(xDelta));
        w.WriteU32LE(static_cast<uint32_t>(yDelta));
    }

    UnitStateDelta UnitStateDelta::Deserialize(ByteReader& r)
    {
        UnitStateDelta d;
        d.ownerSlot = r.ReadU8();
        d.unitLocalIndex = r.ReadU8();

        d.hpDelta = static_cast<int32_t>(r.ReadU32LE());
        d.staminaDelta = static_cast<int32_t>(r.ReadU32LE());
        d.xDelta = static_cast<int32_t>(r.ReadU32LE());
        d.yDelta = static_cast<int32_t>(r.ReadU32LE());
        return d;
    }


    // S2C_BattleResult
    void S2C_BattleResult::Serialize(ByteWriter& w) const
    {
        w.WriteU32LE(runtimeCardId);
        w.WriteU8(ownerSlot);

        // deltas count
        w.WriteU8(static_cast<uint8_t>(deltas.size()));

        for (const auto& d : deltas)
        {
            d.Serialize(w);
        }
    }

    S2C_BattleResult S2C_BattleResult::Deserialize(ByteReader& r)
    {
        S2C_BattleResult pkt;
        pkt.runtimeCardId = r.ReadU32LE();
        pkt.ownerSlot = r.ReadU8();

        uint8_t count = r.ReadU8();
        pkt.deltas.reserve(count);

        for (uint8_t i = 0; i < count; ++i)
        {
            pkt.deltas.push_back(UnitStateDelta::Deserialize(r));
        }

        return pkt;
    }
}
