#pragma once

namespace yuno::net
{
    class ByteWriter;
    class ByteReader;
}

namespace yuno::net::packets
{
    struct RoundUnitInit  final
    {
        uint8_t PID = 0;         // 0이면 에러임
        uint8_t slotID = 0;      // 0이면 에러임
        uint8_t WeaponID = 0;    // 0이면 에러임 >> 어떤 무긴지, 클라쪽도 카드매니저 로드해두래
        uint8_t hp = 0;          // 0이면 에러임
        uint8_t stamina = 0;     // 0이면 에러임
        uint8_t SpawnTileId = 0; // 0이면 에러임

        void Serialize(ByteWriter& w) const;

        static RoundUnitInit Deserialize(ByteReader& r);
    };

    struct S2C_RoundStart final
    {
        // 고정 4개 (P1 2개 + P2 2개)
        RoundUnitInit units[4]{};

        void Serialize(yuno::net::ByteWriter& w) const;
        static S2C_RoundStart Deserialize(yuno::net::ByteReader& r);
    };
}
