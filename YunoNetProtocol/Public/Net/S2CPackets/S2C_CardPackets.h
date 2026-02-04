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
        uint8_t PID;           // 1P,2P
        uint8_t slotID;      // slot ID, 첫번째 무기인지 두번째 무기인지 (1,2)
        uint32_t runtimeID;
        uint32_t dataID;
    };

    struct S2C_StartCardList
    {
        std::vector<CardSpawnInfo> cards;

        void Serialize(ByteWriter& w) const;
        static S2C_StartCardList Deserialize(ByteReader& r);
    };

    struct S2C_DrawCandidates
    {
        std::vector<CardSpawnInfo> cards;

        void Serialize(ByteWriter& w) const;
        static S2C_DrawCandidates Deserialize(ByteReader& r);
    };
}
