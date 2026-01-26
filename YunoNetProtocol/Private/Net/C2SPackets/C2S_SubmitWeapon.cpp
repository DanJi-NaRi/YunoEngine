#include "pch.h"

#include "C2S_SubmitWeapon.h"


namespace yuno::net::packets
{
    void C2S_SubmitWeapon::Serialize(ByteWriter& w) const
    {
        w.WriteU8(WeaponId1);
        w.WriteU8(WeaponId2);
    }

    C2S_SubmitWeapon C2S_SubmitWeapon::Deserialize(ByteReader& r)
    {
        C2S_SubmitWeapon out;
        out.WeaponId1 = r.ReadU8();
        out.WeaponId2 = r.ReadU8();
        return out;
    }
}
