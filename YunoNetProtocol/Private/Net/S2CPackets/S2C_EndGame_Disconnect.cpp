#include "pch.h"

#include "S2C_EndGame_Disconnect.h"

namespace yuno::net::packets
{
    void S2C_EndGame_Disconnect::Serialize(ByteWriter& w) const
    {
        w.WriteU8(winnerPID);
    }

    S2C_EndGame_Disconnect S2C_EndGame_Disconnect::Deserialize(ByteReader& r)
    {
        S2C_EndGame_Disconnect out;

        out.winnerPID = r.ReadU8();

        return out;
    }
}
