#pragma once
#include <vector>
#include <cstdint>

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct UnitStateDelta
    {
        uint8_t ownerSlot;       // 플레이어 식별 (0/1 or 1/2)
        uint8_t unitLocalIndex;  // 해당 플레이어의 유닛

        int8_t hp;
        int8_t stamina;
        uint8_t targetTileID;
        uint8_t dir;

        void Serialize(ByteWriter& w) const;
        static UnitStateDelta Deserialize(ByteReader& r);
    };

    struct S2C_BattleResult
    {
        uint32_t runtimeCardId;
        uint8_t ownerSlot;
        std::vector<UnitStateDelta> deltas;

        void Serialize(ByteWriter& w) const;
        static S2C_BattleResult Deserialize(ByteReader& r);
    };
}
