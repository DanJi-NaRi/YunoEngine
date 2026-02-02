#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct C2S_SubmitWeapon final
    {
        std::uint8_t WeaponId1 = 0;
        std::uint8_t WeaponId2 = 0;

        void Serialize(ByteWriter& w) const;
        static C2S_SubmitWeapon Deserialize(ByteReader& r);
    };
}
