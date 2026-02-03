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

        uint8_t hp;
        uint8_t stamina;
        uint8_t targetTileID;
        uint8_t isEvent;         // 0이면 충돌x, 1이면 충돌o

        void Serialize(ByteWriter& w) const;
        static UnitStateDelta Deserialize(ByteReader& r);
    };

    struct S2C_BattleResult
    {
        uint32_t runtimeCardId;  // 선택한 카드 ID
        uint8_t ownerSlot;       // PID
        uint8_t unitLocalIndex;  // 해당 플레이어의 유닛
        uint8_t dir;             // 카드배치때 선택한 방향 
        uint32_t actionTime;

        std::vector<std::array<UnitStateDelta, 4>> order;   // 순서
        void Serialize(ByteWriter& w) const;
        static S2C_BattleResult Deserialize(ByteReader& r);
    };
}
