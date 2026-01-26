#include "pch.h"

#include "C2S_ReadySet.h"


namespace yuno::net::packets
{
    void C2S_ReadySet::Serialize(ByteWriter& w) const
    {
        w.WriteU8(readyState);
        w.WriteU8(slot1_UnitId);
        w.WriteU8(slot2_UnitId);
    }

    C2S_ReadySet C2S_ReadySet::Deserialize(ByteReader& r)
    {
        C2S_ReadySet out;
        out.readyState = r.ReadU8();
        out.slot1_UnitId = r.ReadU8();
        out.slot2_UnitId = r.ReadU8();
        return out;
    }
}
