#pragma once



namespace yuno::net::packets
{
    struct C2S_ReadyState final
    {
        std::uint8_t readyState = 0; // 0은 준비x, 1은 준비o
        std::uint8_t slot1_UnitId = 0; // 무기는 총 6개 1,2,3,4,5,6 이고 0 은 null처리 정상적인 상황에서 들어오면 안됨
        std::uint8_t slot2_UnitId = 0;

        void Serialize(ByteWriter& w) const;
        static C2S_ReadyState Deserialize(ByteReader& r);
    };
}
