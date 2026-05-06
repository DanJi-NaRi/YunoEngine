#include "pch.h"

#include "S2C_EndGame.h"

namespace yuno::net::packets
{
    void S2C_EndGame::Serialize(ByteWriter& w) const
    {
        for (int i = 0; i < 2; ++i)
        {
            w.WriteU8(results[i].PID);
            w.WriteU8(results[i].winCount);
        }
    }

    S2C_EndGame S2C_EndGame::Deserialize(ByteReader& r)
    {
        S2C_EndGame out;

        for (int i = 0; i < 2; ++i)
        {
            out.results[i].PID = r.ReadU8();
            out.results[i].winCount = r.ReadU8();
        }

        return out;
    }
}
