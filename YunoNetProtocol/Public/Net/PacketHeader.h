#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

#include "PacketType.h"
#include "ByteIO.h"

namespace yuno::net
{
    // 4 BodyLen, 1 PacketType, 1 Version, 2 Reserved
    static constexpr std::size_t yunoPacketHeaderSize = 8;
    // 패킷 최대 크기
    inline constexpr std::uint32_t MaxLength = 4u * 1024u * 1024u;

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
        WriteU32LE(out.data() + 0, bodyLength);

        // Type / Version
        WriteU8(out.data() + 4, static_cast<std::uint8_t>(type));
        WriteU8(out.data() + 5, version);

        // Reserved
        WriteU16LE(out.data() + 6, reserved);

        return out;
    }

    inline PacketHeader UnPackHeaderLE(const std::uint8_t* p)
    {
        PacketHeader h{};

        h.bodyLength = ReadU32LE(p + 0);
        h.type = static_cast<PacketType>(ReadU8(p + 4));
        h.version = ReadU8(p + 5);
        h.reserved = ReadU16LE(p + 6);

        return h;
    }


    inline constexpr bool IsHeaderBodyLengthSane(std::uint32_t bodyLen)
    {
        return bodyLen <= MaxLength;
    }
}
