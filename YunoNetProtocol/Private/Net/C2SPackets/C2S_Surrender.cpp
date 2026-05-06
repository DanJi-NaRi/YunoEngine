#include "pch.h"

#include "C2S_Surrender.h"

namespace yuno::net::packets
{
    void C2S_Surrender::Serialize(ByteWriter& w) const
    {
    }

    C2S_Surrender C2S_Surrender::Deserialize(ByteReader& r)
    {
        C2S_Surrender out;
        return out;
    }
}
