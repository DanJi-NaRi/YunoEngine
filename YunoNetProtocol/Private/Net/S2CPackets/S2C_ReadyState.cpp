#include "pch.h"

#include "S2C_ReadyState.h"


namespace yuno::net::packets
{
    void S2C_ReadyState::Serialize(ByteWriter& w) const
    {
        w.WriteU8(p1_isReady);
        w.WriteU8(p2_isReady);
    }

    S2C_ReadyState S2C_ReadyState::Deserialize(ByteReader& r)
    {
        S2C_ReadyState out;
        out.p1_isReady = r.ReadU8();
        out.p2_isReady = r.ReadU8();
        return out;
    }
}
