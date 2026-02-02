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
        // MK 추가
        // 해당 배틀 결과가 플레이어의 어느 유닛에 의한 행동인지가 필요
        uint8_t unitLocalIndex;  // 해당 플레이어의 유닛
        // 유닛 하나가 죽더라도 패킷에 담아 보내는 걸로 합시다
        // 0번: Player1의 Unit1, 1번: Player1의 Unit2, 2번: Player2의 Unit1, 3번: Player2의 Unit2
        std::vector<std::array<UnitStateDelta, 4>> order;   // 순서
        void Serialize(ByteWriter& w) const;
        static S2C_BattleResult Deserialize(ByteReader& r);
    };
}
