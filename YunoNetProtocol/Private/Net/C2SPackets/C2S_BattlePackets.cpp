#include "pch.h"

#include "C2S_BattlePackets.h"

namespace yuno::net::packets
{
    void C2S_ReadyTurn::Serialize(ByteWriter& w) const
    {
        w.WriteU8(static_cast<uint8_t>(commands.size()));

        for (const auto& cmd : commands)
        {
            w.WriteU32LE(cmd.runtimeID);
            w.WriteU8(static_cast<uint8_t>(cmd.dir));
        }
    }

    C2S_ReadyTurn C2S_ReadyTurn::Deserialize(ByteReader& r)
    {
        C2S_ReadyTurn pkt;

        const uint8_t count = r.ReadU8();
        pkt.commands.reserve(count);

        for (uint8_t i = 0; i < count; ++i)
        {
            CardPlayCommand cmd;
            cmd.runtimeID = r.ReadU32LE();
            cmd.dir = static_cast<Direction>(r.ReadU8());

            pkt.commands.push_back(cmd);
        }

        return pkt;
    }


    void C2S_RoundStartReadyOK::Serialize(ByteWriter& w) const
    {

    }

    C2S_RoundStartReadyOK C2S_RoundStartReadyOK::Deserialize(ByteReader& r)
    {

        {
            return {};
        }
    }
}
