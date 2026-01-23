#include "pch.h"

#include "S2C_Pong.h"


namespace yuno::net::packets
{
    void S2C_Pong::Serialize(ByteWriter& w) const
    {
        w.WriteU32LE(nonce);
    }

    S2C_Pong S2C_Pong::Deserialize(ByteReader& r)
    {
        S2C_Pong out;
        out.nonce = r.ReadU32LE();
        return out;
    }
}
