#pragma once
#include <cstdint>

namespace yuno::net::packets
{
    struct CardInstance
    {
        uint32_t runtimeID;
        uint32_t dataID;
    };
}
