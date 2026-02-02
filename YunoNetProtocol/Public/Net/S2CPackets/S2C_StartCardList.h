#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct CardSpawnInfo
    {
        uint32_t runtimeID;
        uint32_t dataID;
    };
    struct S2C_TestCardList
    {
        std::vector<CardSpawnInfo> cards;

        void Serialize(ByteWriter& w) const;
        static S2C_TestCardList Deserialize(ByteReader& r);
    };
}
