#pragma once

#include "PacketType.h"

namespace yuno::net
{
    class ByteWriter;

    class PacketBuilder final
    {
    public:
        using BuildBodyFn = std::function<void(ByteWriter& w)>;

        static std::vector<std::uint8_t> Build(PacketType type, BuildBodyFn buildBody,
            std::uint8_t version = 1, std::uint16_t reserved = 0);
    };
}
