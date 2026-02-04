#include "pch.h"

#include "S2C_CardPackets.h"

namespace yuno::net::packets
{
    void S2C_StartCardList::Serialize(ByteWriter& w) const
    {
        w.WriteU16LE(static_cast<std::uint16_t>(cards.size()));

        for (const auto& card : cards)
        {
            w.WriteU8(card.slotID);
            w.WriteU8(card.weaponID);
            w.WriteU32LE(card.runtimeID);
            w.WriteU32LE(card.dataID);
        }
    }

    S2C_StartCardList S2C_StartCardList::Deserialize(ByteReader& r)
    {
        S2C_StartCardList out;

        uint16_t count = r.ReadU16LE();
        out.cards.reserve(count);

        for (uint16_t i = 0; i < count; ++i)
        {
            CardSpawnInfo info;
            info.slotID = r.ReadU8();
            info.weaponID = r.ReadU8();
            info.runtimeID = r.ReadU32LE();
            info.dataID = r.ReadU32LE();

            out.cards.push_back(info);
        }

        return out;
    }

    void S2C_DrawCandidates::Serialize(ByteWriter& w) const
    {
        w.WriteU16LE(static_cast<uint16_t>(cards.size()));

        for (const auto& card : cards)
        {
            w.WriteU8(card.slotID);
            w.WriteU8(card.weaponID);
            w.WriteU32LE(card.runtimeID);
            w.WriteU32LE(card.dataID);
        }
    }
    
    S2C_DrawCandidates S2C_DrawCandidates::Deserialize(ByteReader& r)
    {
        S2C_DrawCandidates out;

        uint16_t count = r.ReadU16LE();
        out.cards.reserve(count);

        for (uint16_t i = 0; i < count; ++i)
        {
            CardSpawnInfo info;
            info.slotID = r.ReadU8();
            info.weaponID = r.ReadU8();
            info.runtimeID = r.ReadU32LE();
            info.dataID = r.ReadU32LE();

            out.cards.push_back(info);
        }
        return out;
    }
}
