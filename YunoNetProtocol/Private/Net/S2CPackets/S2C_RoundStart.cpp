#include "pch.h"

#include "S2C_RoundStart.h"


namespace yuno::net::packets
{
    // ----- RoundUnitInit -----
    void RoundUnitInit::Serialize(yuno::net::ByteWriter& w) const
    {
        w.WriteU8(PID);
        w.WriteU8(slotID);
        w.WriteU8(WeaponID);
        w.WriteU8(hp);
        w.WriteU8(stamina);
        w.WriteU8(SpawnTileId);
    }

    RoundUnitInit RoundUnitInit::Deserialize(yuno::net::ByteReader& r)
    {
        RoundUnitInit out{};
        out.PID = r.ReadU8();
        out.slotID = r.ReadU8();
        out.WeaponID = r.ReadU8();
        out.hp = r.ReadU8();
        out.stamina = r.ReadU8();
        out.SpawnTileId = r.ReadU8();
        return out;
    }

    void S2C_RoundStart::Serialize(yuno::net::ByteWriter& w) const
    {
        for (int i = 0; i < 4; ++i)
            units[i].Serialize(w);
    }

    S2C_RoundStart S2C_RoundStart::Deserialize(yuno::net::ByteReader& r)
    {
        S2C_RoundStart out{};
        for (int i = 0; i < 4; ++i)
            out.units[i] = RoundUnitInit::Deserialize(r);
        return out;
    }
}
