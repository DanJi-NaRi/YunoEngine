#pragma once

namespace yuno::net::packets
{
    struct S2C_EnterOK final
    {
        std::uint8_t slotIndex = 0;
        std::uint8_t playerCount = 0;

        void Serialize(ByteWriter& w) const;
        static S2C_EnterOK Deserialize(ByteReader& r);
    };
}
