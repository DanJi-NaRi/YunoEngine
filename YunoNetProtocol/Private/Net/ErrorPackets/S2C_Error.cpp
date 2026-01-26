#include "pch.h"
#include "S2C_Error.h"

namespace yuno::net::packets
{
    void S2C_Error::Serialize(ByteWriter& w) const
    {
        w.WriteU16LE(static_cast<std::uint16_t>(code));
        w.WriteU8(static_cast<std::uint8_t>(reason));
        w.WriteU8(static_cast<std::uint8_t>(contextType));
    }

    S2C_Error S2C_Error::Deserialize(ByteReader& r)
    {
        S2C_Error out;
        out.code = static_cast<ErrorCode>(r.ReadU16LE());
        out.reason = static_cast<EnterDeniedReason>(r.ReadU8());
        out.contextType = static_cast<PacketType>(r.ReadU8());
        return out;
    }
}
