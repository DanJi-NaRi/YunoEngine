#include "pch.h"

#include "S2C_StartCardList.h"

namespace yuno::net::packets
{
    void S2C_TestCardList::Serialize(ByteWriter& w) const
    {
        w.WriteU16LE(static_cast<std::uint16_t>(runtimeIDs.size()));
        for (auto id : runtimeIDs)
        {
            w.WriteU32LE(id);
        }
    }

    S2C_TestCardList S2C_TestCardList::Deserialize(ByteReader& r)
    {
        S2C_TestCardList out;
        uint16_t count = r.ReadU16LE();
        out.runtimeIDs.reserve(count);

        for (uint16_t i = 0; i < count; ++i)
        {
            out.runtimeIDs.push_back(r.ReadU32LE());
        }
        return out;
    }
}
