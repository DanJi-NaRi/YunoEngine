#pragma once

#include <array>

#include "PacketType.h"

namespace yuno::net
{
    // 4 BodyLen, 1 PacketType, 1 Version, 2 Reserved
    static constexpr std::size_t yunoPacketHeaderSize = 8;

    struct PacketHeader
    {
        std::uint32_t bodyLength = 0;
        PacketType    type = PacketType::Invalid;
        std::uint8_t  version = 1; 
        std::uint16_t reserved = 0;
    };


    // =========================
    // Pack / UnPack
    // =========================
    inline std::array<std::uint8_t, yunoPacketHeaderSize>
        PackHeaderLE(std::uint32_t bodyLength, PacketType type, std::uint8_t version = 1, std::uint16_t reserved = 0)
    {
        std::array<std::uint8_t, yunoPacketHeaderSize> out{};

        // BodyLength
        out[0] = static_cast<std::uint8_t>(bodyLength & 0xFFu);
        out[1] = static_cast<std::uint8_t>((bodyLength >> 8) & 0xFFu);
        out[2] = static_cast<std::uint8_t>((bodyLength >> 16) & 0xFFu);
        out[3] = static_cast<std::uint8_t>((bodyLength >> 24) & 0xFFu);

        // Type / Version
        out[4] = static_cast<std::uint8_t>(type);
        out[5] = version;

        // Reserved
        out[6] = static_cast<std::uint8_t>(reserved & 0xFFu);
        out[7] = static_cast<std::uint8_t>((reserved >> 8) & 0xFFu);

        return out;
    }

    inline PacketHeader UnPackHeaderLE(const std::uint8_t* p)
    {
        PacketHeader h{};

        const std::uint32_t bodyLen =
            (static_cast<std::uint32_t>(p[0])) |
            (static_cast<std::uint32_t>(p[1]) << 8) |
            (static_cast<std::uint32_t>(p[2]) << 16) |
            (static_cast<std::uint32_t>(p[3]) << 24);

        h.bodyLength = bodyLen;
        h.type = static_cast<PacketType>(p[4]);
        h.version = p[5];

        const std::uint16_t rsv =
            (static_cast<std::uint16_t>(p[6])) |
            (static_cast<std::uint16_t>(p[7]) << 8);

        h.reserved = rsv;
        return h;
    }

    // =========================
    // Validation helpers (optional but recommended)
    // =========================

    inline constexpr std::uint32_t MaxLength = 4u * 1024u * 1024u;

    inline constexpr bool IsHeaderBodyLengthSane(std::uint32_t bodyLen)
    {
        return bodyLen <= MaxLength;
    }
}
