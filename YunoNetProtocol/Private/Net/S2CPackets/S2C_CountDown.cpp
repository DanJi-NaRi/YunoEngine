#include "pch.h"

#include "S2C_CountDown.h"

namespace yuno::net::packets
{
    void S2C_CountDown::Serialize(ByteWriter& w) const
    {
        w.WriteU8(countTime);
        w.WriteU8(slot1_UnitId1);
        w.WriteU8(slot1_UnitId2);
        w.WriteU8(slot2_UnitId1);
        w.WriteU8(slot2_UnitId2);

    }

    S2C_CountDown S2C_CountDown::Deserialize(ByteReader& r)
    {
        S2C_CountDown out;
        out.countTime = r.ReadU8();

        out.slot1_UnitId1 = r.ReadU8();
        out.slot1_UnitId2 = r.ReadU8();

        out.slot2_UnitId1 = r.ReadU8();
        out.slot2_UnitId2 = r.ReadU8();
        return out;
    }
}
