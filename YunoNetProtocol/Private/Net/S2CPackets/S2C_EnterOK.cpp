#include "pch.h"

#include "S2C_EnterOK.h"

namespace yuno::net::packets
{
    void S2C_EnterOK::Serialize(ByteWriter& w) const
    {
        w.WriteU8(slotIndex);
        w.WriteU8(playerCount); // 디버그용 현재 방에 총 몇명 있는지
    }

    S2C_EnterOK S2C_EnterOK::Deserialize(ByteReader& r)
    {
        S2C_EnterOK out;
        out.slotIndex = r.ReadU8();
        out.playerCount = r.ReadU8();
        return out;
    }
}
