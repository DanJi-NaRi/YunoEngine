#include "pch.h"

#include "S2C_StartCardList.h"

namespace yuno::net::packets
{
    void S2C_TestCardList::Serialize(ByteWriter& w) const
    {
        w.WriteU16LE(static_cast<std::uint16_t>(cards.size()));

        for (const auto& card : cards)
        {
            w.WriteU32LE(card.runtimeID);
            w.WriteU32LE(card.dataID);
        }
    }

    S2C_TestCardList S2C_TestCardList::Deserialize(ByteReader& r)
    {
        S2C_TestCardList out;

        uint16_t count = r.ReadU16LE();
        out.cards.reserve(count);

        for (uint16_t i = 0; i < count; ++i)
        {
            CardSpawnInfo info;
            info.runtimeID = r.ReadU32LE();
            info.dataID = r.ReadU32LE();

            out.cards.push_back(info);
        }

        return out;
    }
}
